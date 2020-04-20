#!/usr/bin/env ruby

#定义代码目录, 此目录下所有文件都会放到src_tree里,
SRC_TREE_DIR = [
	{"dir" => "../../config", "excluded_files" => [],    },
	{"dir" => "../../source", "excluded_files" => [],    },
	{"dir" => "../../test", "excluded_files" => [],    },
	{"dir" => "../../lib/ssz_micro_sys", "excluded_files" => [],    },
	{"dir" => "../../lib/ssz_ui", "excluded_files" => [],    },
]
#定义所有输出项
BUILD_CONFIG = {
	"dest_file_name" => "Chemo-PumpV1_0_master", #目标文件名字
	"dest_file_type" => "bin", #目标类型,有bin,share,static

	"inc_dirs" => [	"../../config",
					"../../source",
					"../../source/common",			
					"../../source/ui_common",
					"../../source/ui_common/resource",
					"../../source/ui_common/widget",
					"../../source/app",
					"../../source/app/screens",
					"../../source/middleware",
					"../../source/middleware/kernel",
					"../../source/driver",
					"../../lib/ssz_micro_sys/bsp",					
					"../../lib/ssz_micro_sys/include",	
					"../../lib/ssz_ui/include",	
					"../../lib/ssz_ui/source",	
					"../../lib/ssz_ui/include/view",	
					"../../lib/ssz_ui/include/view/widget",	
					"../../bsp/bsp_machine/Inc",	
					"../../bsp/bsp_machine/Drivers/CMSIS/Include",	
					"../../bsp/bsp_machine/Drivers/CMSIS/Device/ST/STM32L1xx/Include",
					"../../bsp/bsp_machine/Drivers/STM32L1xx_HAL_Driver/Inc",		
					], #头文件目录
	"macros" => ["STM32L152xC","USE_HAL_DRIVER"], #宏
	"lib_dirs" => [], #库目录
	"share_libs" => [], #动态库文件
	"static_libs" => [], #静态库文件

	#"major_version" => 0,
	# "minor_version" => 0,
	# "revision" => 0,
	# "build_number" => 0,
	"config" => "", #编译配置,自动填充,无需修改
	"platform" => "", #目标平台,自动填充,无需修改
	#源文件只存在config为空的配置中
	#源文件,里面的对象是树结构,一个树对应一个目录,可包含非代码文件,自动填充,无需修改
	#注: 此是默认输出方式
	"src_tree" => [], 
	#源文件,里面的对象是所有srcs路径,可包含非代码文件,自动填充,无需修改
	#注: 此在使用--simple_src时输出
	"src_files" => [], 
}

if $options.build_tool == "vs"
	SRC_TREE_DIR << {"dir" => "../../bsp/bsp_simulator", "excluded_files" => []}
else
	SRC_TREE_DIR << 
	{"dir" => "../../bsp/bsp_machine", 
		"included_files" => ["*.h","*.c","*.s","*.icf"],
		"excluded_dirs" => ["STM32CubeMX","settings"]	
	}
end
#根据生成工具,目标平台,生成配置调整各个配置项
#e.g. vs,debug,win,build_config 
def custom_config(build_tool, config, platform, build_config)

	case config.downcase
	when "release"
		build_config["macros"] += ["NDEBUG"]
	when "debug"
		build_config["macros"] += ["DEBUG","USE_FULL_ASSERT"]
	when "test"
		build_config["macros"] += ["DEBUG", "USE_FULL_ASSERT","TEST"]
	end
	if build_tool == "vs"
		build_config["dest_file_type"] = "static"
		build_config["inc_dirs"] << "../../simulator/driver/cpu_interface"
		build_config["inc_dirs"] << "../../simulator/simulator/include"
		build_config["inc_dirs"] << "../../lib/ssz_micro_sys/bsp/simulator"
		build_config["inc_dirs"] << "../../bsp/bsp_simulator"
		build_config["macros"] << "SSZ_TARGET_SIMULATOR"
	else
		build_config["macros"] << "SSZ_TARGET_MACHINE"
		# build_config["inc_dirs"] << "../../lib/STM32/CMSIS/Include"
		# build_config["inc_dirs"] << "../../lib/STM32/CMSIS/Device/ST/STM32F4xx/Include"
		# build_config["inc_dirs"] << "../../lib/STM32/STM32F4xx_HAL_Driver/Inc"
	end
end

#所有有效的生成配置
VS_ALL_VALID_CONFIG = [
	{:config=>"Debug", :platform=>""},
	{:config=>"Release", :platform=>""},
	{:config=>"Test", :platform=>""},
]

RAKE_ALL_VALID_CONFIG = [
	{:config=>"debug", :platform=>"win"},
	{:config=>"release", :platform=>"win"},
	{:config=>"test", :platform=>"win"},
	{:config=>"debug", :platform=>"unix"},
	{:config=>"release", :platform=>"unix"},
	{:config=>"test", :platform=>"unix"},
	{:config=>"debug", :platform=>"arm"},
	{:config=>"release", :platform=>"arm"},	
	{:config=>"test", :platform=>"arm"},	
	{:config=>"debug", :platform=>"unknow"},
	{:config=>"release", :platform=>"unknow"},		
	{:config=>"test", :platform=>"unknow"},		
]

IAR_ALL_VALID_CONFIG = [
	{:config=>"Debug", :platform=>""},
	{:config=>"Release", :platform=>""},
	{:config=>"Test", :platform=>""},
]

TOOL_CONFIG_MAP = {
	"vs"=>VS_ALL_VALID_CONFIG, 
	"rake"=>RAKE_ALL_VALID_CONFIG,
	"iar"=>IAR_ALL_VALID_CONFIG
}