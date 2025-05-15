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

		Program(const OpenCL::Context& context,
				const OpenCL::Device& device);

		Program(cl_context context,
				cl_device_id device);

		~Program();
	public:
		bool ReadFromFile(const std::filesystem::path& file, 
						  std::string* errMsg = nullptr);
		bool ReadFromString(const std::string& program, 
							std::string* errMsg = nullptr);

		cl_program Get() const { return mpProgram; };
	private:
		bool SetupProgramFromString(const std::string& programString,
									std::string* errMsg);

		std::string ReadProgram(const std::filesystem::path& file,
                                std::unordered_set<std::string>& includedFiles,
								std::string* errMsg);
	private:
		cl_context mpContext;
		cl_device_id mpDeviceId;
		cl_program mpProgram;
	};
}