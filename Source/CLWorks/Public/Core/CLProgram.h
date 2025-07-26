#pragma once

#include "Core/CLContext.h"
#include "Core/CLDevice.h"

#include <filesystem>
#include <string>
#include <unordered_set>

namespace OpenCL
{
	class CLWORKS_API Program
	{
	public:
		Program();

		Program(const std::shared_ptr<Context>& context,
				const std::shared_ptr<Device>& device);

		~Program();
	public:
		cl_program Get() const { return mpProgram; };
	public:
		bool ReadFromFile(const std::filesystem::path& file, 
						  std::string* errMsg = nullptr);
		bool ReadFromString(const std::string& program, 
							std::string* errMsg = nullptr);
	private:
		bool SetupProgramFromString(const std::string& programString,
									std::string* errMsg);

		std::string ReadProgram(const std::filesystem::path& file,
                                std::unordered_set<std::string>& includedFiles,
								std::string* errMsg);
	private:
		cl_program mpProgram;

		std::weak_ptr<Context> mpContext;
		std::weak_ptr<Device> mpDevice;
	};
}