﻿// ==========================================================================
// 
// creepMiner - Burstcoin cryptocurrency CPU and GPU miner
// Copyright (C)  2016-2018 Creepsky (creepsky@gmail.com)
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software Foundation,
// Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110 - 1301  USA
// 
// ==========================================================================

#pragma once

#include <Poco/Task.h>
#include <vector>
#include "Declarations.hpp"
#include <Poco/AutoPtr.h>
#include <Poco/Notification.h>
#include <Poco/NotificationQueue.h>
#include "shabal/MinerShabal.hpp"
#include "logging/Performance.hpp"
#include "mining/Miner.hpp"
#include "logging/Message.hpp"
#include "logging/MinerLogger.hpp"
#include "PlotReader.hpp"
#include "gpu/gpu_shell.hpp"
#include "gpu/algorithm/gpu_algorithm_atomic.hpp"

namespace Burst
{
	class PlotReadProgress;

	struct VerifyNotification : Poco::Notification
	{
		typedef Poco::AutoPtr<VerifyNotification> Ptr;

		std::vector<ScoopData> buffer;
		Poco::UInt64 accountId = 0;
		Poco::UInt64 nonceRead = 0;
		Poco::UInt64 nonceStart = 0;
		std::string inputPath = "";
		Poco::UInt64 block = 0;
		GensigData gensig;
		Poco::UInt64 baseTarget = 0;
		Poco::UInt64 memorySize = 0;
	};
	
	using DeadlineTuple = std::pair<Poco::UInt64, Poco::UInt64>;
	using SubmitFunction = std::function<void(Poco::UInt64, Poco::UInt64, Poco::UInt64, Poco::UInt64, std::string, bool)>;

	template <typename TVerificationAlgorithm>
	class PlotVerifier : public Poco::Task
	{
	public:
		PlotVerifier(MinerData& data, Poco::NotificationQueue& queue, std::shared_ptr<PlotReadProgress> progress,
		             SubmitFunction submitFunction);
		~PlotVerifier() override;
		void runTask() override;
		
	private:
		MinerData* data_;
		Poco::NotificationQueue* queue_;
		std::shared_ptr<PlotReadProgress> progress_;
		SubmitFunction submitFunction_;
	};

	template <typename TVerificationAlgorithm>
	PlotVerifier<TVerificationAlgorithm>::PlotVerifier(MinerData& data, Poco::NotificationQueue& queue,
		std::shared_ptr<PlotReadProgress> progress, SubmitFunction submitFunction)
		: Task("PlotVerifier"), data_{&data}, queue_{&queue}, progress_{progress}, submitFunction_{submitFunction}
	{
	}

	template <typename TVerificationAlgorithm>
	PlotVerifier<TVerificationAlgorithm>::~PlotVerifier()
	{
	}

	template <typename TVerificationAlgorithm>
	void PlotVerifier<TVerificationAlgorithm>::runTask()
	{
		void* stream = nullptr;
		
		if (!TVerificationAlgorithm::initStream(&stream))
		{
			log_critical(MinerLogger::plotVerifier, "Could not create a verification stream!\n"
				"Shutting down the verifier...");
			return;
		}

		while (!isCancelled())
		{
			try
			{
				Poco::Notification::Ptr notification(queue_->waitDequeueNotification());
				VerifyNotification::Ptr verifyNotification;

				if (notification)
					verifyNotification = notification.cast<VerifyNotification>();
				else
					break;

				const auto stopFunction = [this, &verifyNotification]()
				{
					return isCancelled() || verifyNotification->block != data_->getCurrentBlockheight();
				};

				START_PROBE("PlotVerifier.SearchDeadline");
				auto bestResult = TVerificationAlgorithm::run(verifyNotification->buffer, verifyNotification->nonceRead,
					verifyNotification->nonceStart, verifyNotification->baseTarget, verifyNotification->gensig,
					stopFunction, stream);
				TAKE_PROBE("PlotVerifier.SearchDeadline");

				if (bestResult.first != 0 && bestResult.second != 0)
				{
					START_PROBE("PlotVerifier.Submit");
					submitFunction_(bestResult.first,
					                verifyNotification->accountId,
					                bestResult.second,
					                verifyNotification->block,
					                verifyNotification->inputPath,
					                true);
					TAKE_PROBE("PlotVerifier.Submit");
				}

				START_PROBE("PlotVerifier.FreeMemory");
				PlotReader::globalBufferSize.free(verifyNotification->memorySize);
				TAKE_PROBE("PlotVerifier.FreeMemory");

				if (progress_ != nullptr)
					progress_->add(static_cast<Poco::UInt64>(verifyNotification->buffer.size()) * Settings::PlotSize,
						verifyNotification->block);
			}
			catch (Poco::Exception& exc)
			{
				log_error(MinerLogger::plotVerifier, "One of the plot verifiers just crashed! It will recover now.");
				log_exception(MinerLogger::plotVerifier, exc);
			}
			catch (std::exception& exc)
			{
				log_error(MinerLogger::plotVerifier, "One of the plot verifiers just crashed! It will recover now.\n"
					"\tReason:\t%s", std::string(exc.what()));
			}
			catch (...)
			{
				log_error(MinerLogger::plotVerifier, "One of the plot verifiers just crashed by an unknown reason! It will recover now.");
			}
		}

		log_debug(MinerLogger::plotVerifier, "Verifier stopped");
	}

	template <typename TShabal>
	struct PlotVerifierOperations_1
	{
		template <typename TContainer>
		static void updateScoops(TShabal& shabal, const TContainer& scoopPtr)
		{
			shabal.update(scoopPtr[0], Burst::Settings::ScoopSize);
		}

		template <typename TContainer>
		static void close(TShabal& shabal, TContainer& targetPtr)
		{
			shabal.close(targetPtr[0]);
		}
	};

	template <typename TShabal>
	struct PlotVerifierOperations_4
	{
		template <typename TContainer>
		static void updateScoops(TShabal& shabal, const TContainer& scoopPtr)
		{
			shabal.update(scoopPtr[0], scoopPtr[1], scoopPtr[2], scoopPtr[3], Burst::Settings::ScoopSize);
		}

		template <typename TContainer>
		static void close(TShabal& shabal, TContainer& targetPtr)
		{
			shabal.close(targetPtr[0], targetPtr[1], targetPtr[2], targetPtr[3]);
		}
	};

	template <typename TShabal>
	struct PlotVerifierOperations_8
	{
		template <typename TContainer>
		static void updateScoops(TShabal& shabal, const TContainer& scoopPtr)
		{
			shabal.update(scoopPtr[0], scoopPtr[1], scoopPtr[2], scoopPtr[3],
				scoopPtr[4], scoopPtr[5], scoopPtr[6], scoopPtr[7], Burst::Settings::ScoopSize);
		}

		template <typename TContainer>
		static void close(TShabal& shabal, TContainer& targetPtr)
		{
			shabal.close(targetPtr[0], targetPtr[1], targetPtr[2], targetPtr[3],
				targetPtr[4], targetPtr[5], targetPtr[6], targetPtr[7]);
		}
	};

	template <typename TShabal, typename TShabalOperations>
	struct PlotVerifierAlgorithm_cpu
	{
		static bool initStream(void** stream)
		{
			return true;
		}

		static DeadlineTuple run(std::vector<ScoopData>& buffer, Poco::UInt64 nonceRead,
						Poco::UInt64 nonceStart, Poco::UInt64 baseTarget, const GensigData& gensig,
						std::function<bool()> stop, void* stream)
		{
			DeadlineTuple bestResult = {0, 0};
			TShabal shabal;

			// hash the gensig according to the cpu instruction level
			shabal.update(gensig.data(), Settings::HashSize);

			for (size_t i = 0;
				i < buffer.size() && !stop();
				i += TShabal::HashSize)
			{
				auto result = verify(shabal, buffer, nonceRead, nonceStart, i, baseTarget);

				for (auto& pair : result)
					// make sure the nonce->deadline pair is valid...
					if (pair.first > 0 && pair.second > 0)
						// ..and better than the others
						if (bestResult.second == 0 || pair.second < bestResult.second)
							bestResult = pair;
			}

			return bestResult;
		}

		static std::vector<DeadlineTuple> verify(const TShabal& shabalCopy, std::vector<ScoopData>& buffer, Poco::UInt64 nonceRead,
										  Poco::UInt64 nonceStart, size_t offset, Poco::UInt64 baseTarget)
		{
			constexpr auto HashSize = TShabal::HashSize;
			TShabal shabal = shabalCopy;

			std::array<HashData, HashSize> targets;
			std::array<Poco::UInt64, HashSize> results;

			// these are the buffer overflow prove arrays 
			// instead of directly working with the raw arrays  
			// we create an additional level of indirection 
			std::array<const unsigned char*, HashSize> scoopPtr;
			std::array<unsigned char*, HashSize> targetPtr;

			// we init the buffer overflow guardians
			for (size_t i = 0; i < HashSize; ++i)
			{
				const auto overflow = i + offset >= buffer.size();

				// if the index would cause a buffer overflow, we init it 
				// with a nullptr, otherwise with the value
				scoopPtr[i] = overflow ? nullptr : reinterpret_cast<unsigned char*>(buffer.data() + offset + i);
				targetPtr[i] = overflow ? nullptr : reinterpret_cast<unsigned char*>(targets[i].data());
			}

			// hash the scoop according to the cpu instruction level
			TShabalOperations::updateScoops(shabal, scoopPtr);

			// digest the hash
			TShabalOperations::close(shabal, targetPtr);

			for (auto i = 0u; i < HashSize; ++i)
				memcpy(&results[i], targets[i].data(), sizeof(Poco::UInt64));

			// for every calculated deadline we create a pair of {nonce->deadline}
			std::vector<DeadlineTuple> pairs(HashSize);

			for (size_t i = 0u; i < HashSize; ++i)
				// only set the pair if it was calculated
				if (i + offset < buffer.size())
					pairs[i] = std::make_pair(nonceStart + nonceRead + offset + i, results[i] / baseTarget);

			return pairs;
		}
	};

	template <typename TGpu, typename TAlgorithm>
	struct PlotVerifierAlgorithm_gpu
	{
		static bool initStream(void** stream)
		{
			return TGpu::initStream(stream);
		}

		static DeadlineTuple run(std::vector<ScoopData>& buffer, Poco::UInt64 nonceRead,
			Poco::UInt64 nonceStart, Poco::UInt64 baseTarget, const GensigData& gensig,
			std::function<bool()> stop, void* stream)
		{
			DeadlineTuple bestDeadline{0, 0};
			TGpu::template run<TAlgorithm>(
				buffer,
				gensig,
				nonceStart + nonceRead,
				baseTarget,
				stream,
				bestDeadline);
			return bestDeadline;
		}
	};

	using PlotVerifierOperation_sse2 = PlotVerifierOperations_1<Shabal256_SSE2>;
	using PlotVerifierOperation_sse4 = PlotVerifierOperations_4<Shabal256_SSE4>;
	using PlotVerifierOperation_avx = PlotVerifierOperations_4<Shabal256_AVX>;
	using PlotVerifierOperation_avx2 = PlotVerifierOperations_8<Shabal256_AVX2>;
	using PlotVerifierOperation_neon = PlotVerifierOperations_4<Shabal256_NEON>;

	using PlotVerifierAlgorithm_sse2 = PlotVerifierAlgorithm_cpu<Shabal256_SSE2, PlotVerifierOperation_sse2>;
	using PlotVerifierAlgorithm_sse4 = PlotVerifierAlgorithm_cpu<Shabal256_SSE4, PlotVerifierOperation_sse4>;
	using PlotVerifierAlgorithm_avx = PlotVerifierAlgorithm_cpu<Shabal256_AVX, PlotVerifierOperation_avx>;
	using PlotVerifierAlgorithm_avx2 = PlotVerifierAlgorithm_cpu<Shabal256_AVX2, PlotVerifierOperation_avx2>;
	using PlotVerifierAlgorithm_neon = PlotVerifierAlgorithm_cpu<Shabal256_NEON, PlotVerifierOperation_neon>;

	using PlotVerifier_sse2 = PlotVerifier<PlotVerifierAlgorithm_sse2>;
	using PlotVerifier_sse4 = PlotVerifier<PlotVerifierAlgorithm_sse4>;
	using PlotVerifier_avx = PlotVerifier<PlotVerifierAlgorithm_avx>;
	using PlotVerifier_avx2 = PlotVerifier<PlotVerifierAlgorithm_avx2>;
	using PlotVerifier_neon = PlotVerifier<PlotVerifierAlgorithm_neon>;

	using PlotVerifierAlgorithm_cuda = PlotVerifierAlgorithm_gpu<GpuCuda, Gpu_Algorithm_Atomic>;
	using PlotVerifierAlgorithm_opencl = PlotVerifierAlgorithm_gpu<GpuOpenCL, Gpu_Algorithm_Atomic>;

	using PlotVerifier_cuda = PlotVerifier<PlotVerifierAlgorithm_cuda>;
	using PlotVerifier_opencl = PlotVerifier<PlotVerifierAlgorithm_opencl>;
}

