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

#include <Poco/Types.h>
#include <map>

namespace Burst
{
	/**
	 * \brief The output ids.
	 * They all represent a specific event in the application.
	 */
	enum Output : Poco::UInt32
	{
		LastWinner,
		NonceFound,
		NonceFoundTooHigh,
		NonceSent,
		NonceConfirmed,
		PlotDone,
		DirDone
	};

	/**
	 * \brief Alias for std::map<Output, T>.
	 * \tparam T The value type.
	 */
	template <typename T>
	using OutputMap = std::map<Output, T>;

	/**
	 * \brief Alias for std::map<Output, bool>.
	 */
	using OutputFlags = OutputMap<bool>;

	/**
	 * \brief Helper class for \enum Output.
	 */
	struct OutputHelper
	{
		/**
		 * \brief A map with canonical names for all values in \enum Output.
		 */
		static const OutputMap<std::string> outputNames;

		/**
		 * \brief Gets the canonical name for a output.
		 * \param output The output id.
		 * \return The canonical name of the output, if it exists. An empty string otherwise.
		 */
		static std::string outputToString(Output output);

		/**
		 * \brief Gets the matching \enum Output for his canonical name.
		 * \param output The canonical name of the output.
		 * \return The \enum Output, if it exists.
		 * Otherwise the default value of \enum Output (undefined).
		 */
		static Output stringToOutput(const std::string& output);

		/**
		 * \brief Creates a flag map for all values inside \enum Output.
		 * \param defaultFlag The value of the flags.
		 * \return The flag map.
		 */
		static OutputFlags createFlags(bool defaultFlag = true);
	};	
}
