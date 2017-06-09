﻿#pragma once

#include "Deadline.hpp"
#include <Poco/Timestamp.h>
#include <Poco/Timespan.h>
#include <Poco/JSON/Object.h>
#include <Poco/Mutex.h>
#include <deque>
#include <Poco/NotificationCenter.h>
#include <Poco/Observer.h>
#include <Poco/ActiveDispatcher.h>
#include <Poco/ActiveMethod.h>
#include <unordered_map>
#include <atomic>
#include <functional>

namespace Burst
{
	class MinerData;
	class Accounts;
	class Wallet;
	class Account;

	class BlockData : public Poco::ActiveDispatcher
	{
	public:
		enum class DeadlineSearchType
		{
			Found,
			Sent,
			Confirmed
		};

	public:
		BlockData(uint64_t blockHeight, uint64_t baseTarget, std::string genSigStr, MinerData* parent = nullptr);

		std::shared_ptr<Deadline> addDeadline(uint64_t nonce, uint64_t deadline,
			std::shared_ptr<Account> account, uint64_t block, std::string plotFile);
		void setBaseTarget(uint64_t baseTarget);
		void setLastWinner(std::shared_ptr<Account> account);
		
		void refreshBlockEntry() const;
		void setProgress(float progress, uint64_t blockheight);

		uint64_t getBlockheight() const;
		uint64_t getScoop() const;
		uint64_t getBasetarget() const;
		std::shared_ptr<Account> getLastWinner() const;
		Poco::ActiveResult<std::shared_ptr<Account>> getLastWinnerAsync(const Wallet& wallet, const Accounts& accounts);
		const GensigData& getGensig() const;
		const std::string& getGensigStr() const;
		std::shared_ptr<Deadline> getBestDeadline() const;
		//std::vector<Poco::JSON::Object> getEntries() const;
		bool forEntries(std::function<bool(const Poco::JSON::Object&)> traverseFunction) const;
		//const std::unordered_map<AccountId, Deadlines>& getDeadlines() const;
		std::shared_ptr<Deadline> getBestDeadline(uint64_t accountId, DeadlineSearchType searchType);
		
		std::shared_ptr<Deadline> addDeadlineIfBest(uint64_t nonce, uint64_t deadline,
			std::shared_ptr<Account> account, uint64_t block, std::string plotFile);

	protected:
		std::shared_ptr<Account> runGetLastWinner(const std::pair<const Wallet*, const Accounts*>& args);
		void addBlockEntry(Poco::JSON::Object entry) const;
		void confirmedDeadlineEvent(std::shared_ptr<Deadline> deadline);

	private:
		std::atomic<uint64_t> blockHeight_;
		std::atomic<uint64_t> scoop_;
		std::atomic<uint64_t> baseTarget_;
		GensigData genSig_;
		std::string genSigStr_ = "";
		std::shared_ptr<std::vector<Poco::JSON::Object>> entries_;
		std::shared_ptr<Account> lastWinner_ = nullptr;
		std::unordered_map<AccountId, Deadlines> deadlines_;
		std::shared_ptr<Deadline> bestDeadline_;
		Poco::ActiveMethod<std::shared_ptr<Account>, std::pair<const Wallet*, const Accounts*>, BlockData,
			Poco::ActiveStarter<ActiveDispatcher>> activityLastWinner_;
		MinerData* parent_;
		Poco::JSON::Object::Ptr jsonProgress_;
		mutable Poco::Mutex mutex_;

		friend class Deadlines;
	};

	struct BlockDataChangedNotification : Poco::Notification
	{
		BlockDataChangedNotification(Poco::JSON::Object* blockData)
			: blockData{blockData} {}
		~BlockDataChangedNotification() override = default;
		Poco::JSON::Object* blockData;
	};

	class MinerData
	{
	public:
		MinerData();
		
		std::shared_ptr<BlockData> startNewBlock(uint64_t block, uint64_t baseTarget, const std::string& genSig);
		void setTargetDeadline(uint64_t deadline);

		std::shared_ptr<Deadline> getBestDeadlineOverall() const;
		const Poco::Timestamp& getStartTime() const;
		Poco::Timespan getRunTime() const;
		uint64_t getBlocksMined() const;
		uint64_t getBlocksWon() const;
		std::shared_ptr<BlockData> getBlockData();
		std::shared_ptr<const BlockData> getBlockData() const;
		std::shared_ptr<const BlockData> getHistoricalBlockData(uint32_t roundsBefore) const;
		std::vector<std::shared_ptr<const BlockData>> getAllHistoricalBlockData() const;
		uint64_t getConfirmedDeadlines() const;
		uint64_t getTargetDeadline() const;
		bool compareToTargetDeadline(uint64_t deadline) const;
		uint64_t getAverageDeadline() const;

		uint64_t getCurrentBlockheight() const;
		uint64_t getCurrentBasetarget() const;
		uint64_t getCurrentScoopNum() const;

		template <typename Observer>
		void addObserverBlockDataChanged(Observer& observer,
			typename Poco::Observer<Observer, BlockDataChangedNotification>::Callback function)
		{
			notifiyBlockDataChanged_.addObserver(Poco::Observer<Observer, BlockDataChangedNotification>{
				observer, function
			});
		}

	private:
		void addWonBlock();
		void addConfirmedDeadline();
		void setBestDeadline(std::shared_ptr<Deadline> deadline);

		Poco::Timestamp startTime_ = {};
		std::shared_ptr<Deadline> bestDeadlineOverall_ = nullptr;
		std::atomic<uint64_t> blocksMined_;
		std::atomic<uint64_t> blocksWon_;
		std::atomic<uint64_t> deadlinesConfirmed_;
		std::atomic<uint64_t> targetDeadline_;
		std::shared_ptr<BlockData> blockData_ = nullptr;
		std::deque<std::shared_ptr<BlockData>> historicalBlocks_;
		mutable Poco::Mutex mutex_;
		Poco::NotificationCenter notifiyBlockDataChanged_;

		std::atomic<uint64_t> currentBlockheight_;
		std::atomic<uint64_t> currentBasetarget_;
		std::atomic<uint64_t> currentScoopNum_;

		friend class BlockData;
	};
}
