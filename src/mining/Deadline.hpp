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

#include <memory>
#include "Declarations.hpp"
#include <Poco/Mutex.h>
#include <atomic>
#include <set>
#include <vector>
#include <Poco/Net/IPAddress.h>

namespace Burst
{
	class Deadlines;
	class Account;
	class BlockData;

	class Deadline : public std::enable_shared_from_this<Deadline>
	{
	public:

		Deadline(Poco::UInt64 nonce, Poco::UInt64 deadline, std::shared_ptr<Account> account, Poco::UInt64 block, std::string plotFile,
			Deadlines* parent = nullptr);
		Deadline(const Deadline& other) = delete;
		Deadline(Deadline&& other) noexcept;
		Deadline& operator=(const Deadline& other) = delete;
		Deadline& operator=(Deadline&& other) noexcept;
		~Deadline();

		void found(bool tooHigh = false);
		void onTheWay();
		void send();
		void confirm();

		std::string deadlineToReadableString() const;
		Poco::UInt64 getNonce() const;
		Poco::UInt64 getDeadline() const;
		AccountId getAccountId() const;
		std::string getAccountName() const;
		Poco::UInt64 getBlock() const;
		const std::string& getPlotFile() const;
		std::string getMiner() const;
		const std::string& getWorker() const;
		Poco::UInt64 getTotalPlotsize() const;
		const Poco::Net::IPAddress& getIp() const;
		std::string toActionString(const std::string& action) const;
		std::string toActionString(const std::string& action,
			const std::vector<std::pair<std::string, std::string>>& additionalData) const;

		bool isOnTheWay() const;
		bool isSent() const;
		bool isConfirmed() const;

		void setDeadline(Poco::UInt64 deadline);
		void setMiner(const std::string& miner);
		void setWorker(const std::string& worker);
		void setTotalPlotsize(Poco::UInt64 plotsize);
		void setIp(const Poco::Net::IPAddress& ip);
		void setParent(Deadlines* parent);

		bool operator<(const Deadline& rhs) const;
		bool operator()(const Deadline& lhs, const Deadline& rhs) const;

	private:
		std::shared_ptr<Account> account_;
		Poco::UInt64 block_;
		Poco::UInt64 nonce_;
		Poco::UInt64 deadline_;
		std::string plotFile_ = "";
		std::atomic<bool> onTheWay_;
		std::atomic<bool> sent_;
		std::atomic<bool> confirmed_;
		std::string minerName_ = "";
		std::string workerName_ = "";
		Poco::UInt64 plotsize_ = 0;
		Poco::Net::IPAddress ip_{"127.0.0.1"};
		Deadlines* parent_;
		mutable Poco::FastMutex mutex_;
	};

	class Deadlines
	{
	public:
		explicit Deadlines(BlockData* parent = nullptr);
		//Deadlines(const Deadlines& rhs);
		//Deadlines& operator=(const Deadlines& rhs);

		std::shared_ptr<Deadline> add(Poco::UInt64 nonce, Poco::UInt64 deadline, std::shared_ptr<Account> account, Poco::UInt64 block, std::string plotFile);
		void add(std::shared_ptr<Deadline> deadline);
		void clear();
		bool confirm(Nonce nonce);
		bool confirm(Nonce nonce, AccountId accountId, Poco::UInt64 block);

		std::shared_ptr<Deadline> getBest() const;
		std::shared_ptr<Deadline> getBestConfirmed() const;
		std::shared_ptr<Deadline> getBestFound() const;
		std::shared_ptr<Deadline> getBestSent() const;

		std::vector<std::shared_ptr<Deadline>> getDeadlines() const;

	private:
		void deadlineEvent(const std::shared_ptr<Deadline>& deadline, const std::string& type) const;
		void deadlineConfirmed(const std::shared_ptr<Deadline>& deadline) const;
		void resort();

		struct LessThan : std::binary_function<std::shared_ptr<Deadline>, std::shared_ptr<Deadline>, bool>
		{
			bool operator()(const std::shared_ptr<Deadline>& lhs, const std::shared_ptr<Deadline>& rhs) const
			{
				return !(lhs == rhs) && (*lhs < *rhs);
			}
		};

		std::set<std::shared_ptr<Deadline>, LessThan> deadlines_;
		BlockData* parent_;
		mutable Poco::FastMutex mutex_;

		friend class Deadline;
	};
}
