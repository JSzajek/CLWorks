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
		Program(const OpenCL::Device& device,
				const OpenCL::Context& context);

		Program(cl_device_id device, 
				cl_context contex);

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
		cl_device_id mDeviceId;
		cl_context mContext;
		cl_program mpProgram;
	};
}