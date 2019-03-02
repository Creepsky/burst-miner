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

#include "ProgressPrinter.hpp"
#include "Console.hpp"
#include "MinerLogger.hpp"
#include "MinerUtil.hpp"
#include "mining/MinerConfig.hpp"

Burst::ProgressPrinter::ProgressPrinter()
{
#ifdef POCO_COMPILER_MSVC
	delimiterFront = { "\xBA", TextType::Unimportant };
	readDoneChar = { "\xB1", TextType::Normal };
	verifiedDoneChar = { "\xB2", TextType::Success };
	readNotDoneChar = { "\xB0", TextType::Unimportant };
	delimiterEnd = { "\xBA", TextType::Unimportant };
#else
	delimiterFront = { "\u2590", TextType::Unimportant };
	readDoneChar = { "\u2592", TextType::Success };
	verifiedDoneChar = { "\u2593", TextType::Ok };
	readNotDoneChar = { "\u2591", TextType::Unimportant };
	delimiterEnd = { "\u258C", TextType::Unimportant };
#endif
	totalSize = 48;
}

namespace Burst
{
	std::ostream& toPercentage(std::ostream& stream)
	{
		stream << std::right << std::fixed << std::setw(6) << std::setfill(' ') << std::setprecision(2);
		return stream;
	}

	std::string repeat(size_t times, const std::string&token)
	{
		std::stringstream sstream;

		for (auto i = 0u; i < times; ++i)
			sstream << token;

		return sstream.str();
	}
}

void Burst::ProgressPrinter::print(const Progress& progress) const
{
	if (MinerConfig::getConfig().getLogOutputType() != LogOutputType::Terminal)
		return;

	if (MinerConfig::getConfig().isFancyProgressBar())
	{
		// calculate the progress bar proportions
		size_t doneSizeRead, notDoneSize, doneSizeVerified;

		calculateProgressProportions(progress.read, progress.verify, totalSize, doneSizeRead, doneSizeVerified, notDoneSize);

		auto block = Console::print();
		
		block << MinerLogger::getTextTypeColor(TextType::Unimportant) << getTime() << ": "
			<< MinerLogger::getTextTypeColor(delimiterFront.textType) << delimiterFront.character
			<< MinerLogger::getTextTypeColor(verifiedDoneChar.textType) << repeat(doneSizeVerified, verifiedDoneChar.character)
			<< MinerLogger::getTextTypeColor(readDoneChar.textType) << repeat(doneSizeRead, readDoneChar.character)
			<< MinerLogger::getTextTypeColor(readNotDoneChar.textType) << repeat(notDoneSize, readNotDoneChar.character)
			<< MinerLogger::getTextTypeColor(delimiterEnd.textType) << delimiterEnd.character
			<< ' ' << toPercentage << (progress.read + progress.verify) / 2 << " % "
			<< MinerLogger::getTextTypeColor(delimiterEnd.textType) << delimiterEnd.character
			<< ' ' << memToString(static_cast<Poco::UInt64>(progress.bytesPerSecondRead), 2) << "/s";

		block.flush();
	}
	else
		Console::print()
			<< MinerLogger::getTextTypeColor(TextType::Unimportant) << getTime() << ": "
			<< "Read:  " << toPercentage << progress.read << "% ("
			<< memToString(static_cast<Poco::UInt64>(progress.bytesPerSecondRead), 2) << "/s)   "
			<< "Verified:  " << toPercentage << progress.verify << "% ("
			<< memToString(static_cast<Poco::UInt64>(progress.bytesPerSecondVerify), 2) << "/s)";
}

void Burst::ProgressPrinter::calculateProgressProportions(double progressRead, double progressVerified,
														  size_t totalSize, size_t& readSize, size_t& verifiedSize, size_t& notDoneSize)
{
	readSize = static_cast<size_t>(totalSize * (progressRead / 100));
	verifiedSize = static_cast<size_t>(totalSize * (progressVerified / 100));

	auto notReadSize = totalSize - readSize;
	auto notVerifiedSize = totalSize - verifiedSize;

	if (readSize > verifiedSize)
		readSize = readSize - verifiedSize;
	else
		readSize = 0;

	notDoneSize = std::min(notReadSize, notVerifiedSize);
}
