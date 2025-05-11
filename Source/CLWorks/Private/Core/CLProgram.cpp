#include "Core/CLProgram.h"

#include "CLWorksLog.h"

#include <sstream>
#include <fstream>

namespace OpenCL
{
	Program::Program(const Device& device, 
					 const Context& context)
		: mDeviceId(device.Get()),
		mContext(context.Get()),
		mpProgram(nullptr)
	{
	}

	Program::Program(cl_device_id device,
					 cl_context context)
		: mDeviceId(device),
		mContext(context),
		mpProgram(nullptr)
	{
	}

	Program::~Program()
	{
		if (mpProgram)
		{
			clReleaseProgram(mpProgram);
			mpProgram = nullptr;
		}
	}

	void Program::ReadFromFile(const std::filesystem::path& file)
	{
		std::unordered_set<std::string> includedFiles;
		const std::string program_string = ReadProgram(file, includedFiles);

		SetupProgramFromString(program_string);
	}

	void Program::ReadFromString(const std::string& program)
	{
		SetupProgramFromString(program);
	}

	void Program::SetupProgramFromString(const std::string& programString)
	{
		cl_program program;
		char* program_buffer, * program_log;
		size_t program_size, log_size;

		int32_t err = 0;

		program_size = programString.size();
		program_buffer = (char*)malloc(program_size + 1);
		program_buffer[program_size] = '\0';
		std::memcpy(program_buffer, programString.c_str(), program_size);

		program = clCreateProgramWithSource(mContext, 
											1,
											(const char**)&program_buffer, 
											&program_size,
											&err);

		if (err < 0)
		{
			UE_LOG(LogCLWorks, Error, TEXT("Couldn't Create the Program!"));
			return;
		}
		free(program_buffer);

		err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
		if (err < 0) 
		{
			/* Find size of log and print to std output */
			clGetProgramBuildInfo(program, 
								  mDeviceId, 
								  CL_PROGRAM_BUILD_LOG,
								  0, 
								  NULL, 
								  &log_size);

			program_log = (char*)malloc(log_size + 1);
			program_log[log_size] = '\0';

			clGetProgramBuildInfo(program, 
								  mDeviceId, 
								  CL_PROGRAM_BUILD_LOG,
								  log_size + 1, 
								  program_log, 
								  NULL);

			UE_LOG(LogCLWorks, Error, TEXT("Program Error: %s"), *FString(program_log));
			free(program_log);
			return;
		}

		mpProgram = program;
	}

	std::string Program::ReadProgram(const std::filesystem::path& file,
									 std::unordered_set<std::string>& includedFiles)
	{
		if (!std::filesystem::exists(file))
		{
			UE_LOG(LogCLWorks, Error, TEXT("File Couldn't Be Found At: %s"), *FString(file.c_str()));
			return "";
		}

		/* Read program file and place content into buffer */
		std::ifstream program_file;
		program_file.open(file);

		if (!program_file.is_open()) 
		{
			UE_LOG(LogCLWorks, Error, TEXT("Could Not Open File: %s"), *FString(file.c_str()));
			return "";
		}

		std::stringstream buffer;
		buffer << program_file.rdbuf();
		std::string program_string = buffer.str();
		if (program_string.find("#include") != std::string::npos)
		{
			std::istringstream iss(program_string);
			std::string line;
			while (std::getline(iss, line))
			{
				if (line.find("#include") != std::string::npos)
				{
					std::string includeFile = line.substr(line.find_first_of("\"") + 1, line.find_last_of("\"") - line.find_first_of("\"") - 1);
					if (includedFiles.find(includeFile) == includedFiles.end())
					{
						includedFiles.insert(includeFile);
						std::filesystem::path includeFilePath = file.parent_path() / includeFile;
						std::string includeFileContent = ReadProgram(includeFilePath, includedFiles);
						program_string.replace(program_string.find(line), line.size(), includeFileContent);
					}
				}
			}
		}
		return program_string;
	}
}