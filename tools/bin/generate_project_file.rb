#!/usr/bin/env ruby
#require "profile" #使用这个库可以分析性能
#require 'benchmark' #通过以下语句可以测试代码运行时间: Benchmark.measure {运行代码 }

#添加系统中的库
#require 'fileutils'
require 'yaml'
require "pp"

#添加同目录的库
#下面的库可以分析一些共同的命令行参数以及添加一些通用的函数等
if File.file? File.dirname(__FILE__)+"/"+"main_common.rb"
	require_relative "main_common"  
else
	#如果同目录下没有就试试系统库$LOAD_PATH
	require 'main_common'
end

#设置帮助,版本等信息
SELF_NAME = File.basename(__FILE__)
HELP_STR = %Q{This is used to generate VS, IAR project file
Usage: #{SELF_NAME} [option]... <file>...	
Note: <file> can use wildcard, #{CommonOptionParser::WILDCARD_USE_HELP}}
EXAMPLE_STR = %Q{Example:
#{SELF_NAME} -p project_template -b build_config.rb
}		
VERSION_STR = "1.0.0"
ERROR_INFO_WHEN_NO_IN_ARG = "no input file"

#初始化所有option
$options = OpenStruct.new
$options.out_file_name = nil   #输出文件名字, 如果不定义此变量,则命令选项-o,--outfile没有
$options.error_out_file_name = nil   #错误输出文件名字,如果不定义此变量,则命令选项--error-outfile没有
$options.list_file_name = nil  #包含文件/目录名列表的文件名字,在列表中的文件/目录名不能包含通配符,也不能递归查找,
								#如果不定义此变量,则命令选项--listfile没有
$options.is_recursive = nil  #是否也处理在子目录中的输入文件,如果不定义此变量,则命令选项-r,--recursive没有
$options.is_allow_wildcard = true #是否允许参数为通配符
$options.action_when_no_normal_arg = "nil" #当没有普通参数时的动作,"error":报错, "help":打印帮助,nil:正常
$options.is_use_stdin_as_inputfile = nil #是否从stdin读取输入内容,如果不定义此变量,则命令选项--from-stdin没有
$options.normal_args = []  #所要处理的输入参数(此参数数组中不包含options,参数可能是文件,目录名或其他)
#其他选项
$options.project_template_file_path = nil
$options.project_type = nil
$options.build_config_file_path = nil


#配置选项并根据选项解析参数
#根据$options的值添加选项: -o, --outfile=<file>,--error-outfile=<file>, --listfile=<file>,
#	       -r,--recursive,-h,--help,-v,--version
CommonOptionParser.analyze_opt($options, HELP_STR, EXAMPLE_STR, VERSION_STR, ERROR_INFO_WHEN_NO_IN_ARG) do |opts|
	#添加自定义的选项
	# # 第一项是Short option，第二项是Long option，第三项是对Option的描述
	opts.on("-p", "--project_template=<file>",
	"input project template file") do |file_name|
		$options.project_template_file_path = file_name
	end

	opts.on("-t", "--project_type=<type>",
	"the project type") do |file_name|
		$options.project_type = file_name
	end
	opts.on("-b", "--build_config=<file>",
	"buid config file") do |file_name|
		$options.build_config_file_path = file_name
	end
	# 
end

def get_out_file 
	if $options.out_file_name
		return File.open $options.out_file_name,"w"
	else
		return $stdout
	end
end

#函数定义
def find_config(config_str, platform_str)
	$all_config.each do |config|
		if config["config"].casecmp(config_str) == 0 && config["platform"].casecmp(platform_str)==0
			return config
		end
	end

	return nil
end

def get_macro_str(macros, project_type, prefix_spaces)

	ret = ""
	macros.each_with_index do |var,index|
		case project_type.downcase
		when "iar"
			if index > 0
				#非第一行需要带缩进
				ret += "#{prefix_spaces}"				
			end
			ret += "<state>#{var}</state>\n"
		when "vs"
			ret += "#{var};"
		else
			raise("unkonw project type:#{project_type}")
		end
	end

	return ret
end

def get_inc_str(incs, project_type, prefix_spaces)

	ret = ""
	incs.each_with_index do |var,index|
		case project_type.downcase
		when "iar"
			if index > 0
				#非第一行需要带缩进
				ret += "#{prefix_spaces}"				
			end
			var = var.gsub("/","\\")
			var.insert(0, "$PROJ_DIR$\\")
			ret += "<state>#{var}</state>\n"
		when "vs"
			var.gsub!("/","\\")
			ret += "#{var};"
		else
			raise("unkonw project type:#{project_type}")
		end
	end

	return ret
end

def get_share_lib_str(share_libs, project_type, prefix_spaces)

	ret = ""
	share_libs.each_with_index do |var,index|
		case project_type.downcase
		when "vs"
			ret += "#{var}.lib;"
		else
			raise("unkonw project type:#{project_type}")
		end
	end

	return ret
end

def get_static_lib_str(static_libs, project_type, prefix_spaces, lib_dirs)

	ret = ""
	if lib_dirs.size >0
		lib_dir = lib_dirs[0].gsub("/","\\")
		lib_dir.insert(0, "$PROJ_DIR$\\")
	else
		lib_dir = "$PROJ_DIR$\\"
	end
	static_libs.each_with_index do |var,index|
		case project_type.downcase
		when "vs"
			ret += "#{var}.lib;"
		when "iar"
			if index > 0
				#非第一行需要带缩进
				ret += "#{prefix_spaces}"				
			end
			ret += "<state>#{lib_dir}\\#{var}.a</state>\n"
		else
			raise("unkonw project type:#{project_type}")
		end
	end

	return ret
end
#生成树结构的字符串,前面需要都有prefix_spaces缩进
def get_src_tree_str(parent_path,src_tree, project_type, prefix_spaces)
	ret = ""
	tab_str = "  "
	ret ="#{prefix_spaces}<group>\n"
	ret +="#{prefix_spaces}#{tab_str}<name>#{src_tree['name']}</name>\n"

	if parent_path == nil
		new_parent_path = src_tree['name']
	else
		new_parent_path = parent_path +"/"+src_tree['name']
	end
	src_tree['childs'].each do |v|
		ret += get_src_tree_str(new_parent_path, v, project_type, prefix_spaces+tab_str)
	end

	src_tree['files'].each do |v|
		v = v.gsub("/","\\")
		v.insert(0, "$PROJ_DIR$\\")
		ret += "#{prefix_spaces}#{tab_str}<file>\n"
		ret += "#{prefix_spaces}#{tab_str}#{tab_str}<name>#{v}</name>\n"
		ret += "#{prefix_spaces}#{tab_str}</file>\n"
	end


	ret +="#{prefix_spaces}</group>\n"
	return ret
end

#生成所有平铺的字符串,前面需要都有prefix_spaces缩进, type: "head_file", "src_file", "dir"
def get_src_all_str(parent_path, src_tree, project_type, prefix_spaces, type, is_for_filter)
	ret = ""

	#处理目录
	if parent_path == nil
		new_parent_path = src_tree['name']
	else
		new_parent_path = parent_path +"/"+src_tree['name']
	end
	new_parent_path_at_os = new_parent_path.gsub("/", "\\")
	if type == "dir"
		ret += "#{prefix_spaces}<Filter Include=\"#{new_parent_path_at_os}\">\n"
		ret += "#{prefix_spaces}</Filter>\n"		
	end
	src_tree['childs'].each do |v|
		ret += get_src_all_str(new_parent_path, v, project_type, prefix_spaces, type, is_for_filter)
	end

	#处理目录中的文件
	if type == "head_file"
		tag = "ClInclude"
	elsif type == "src_file"
		tag = "ClCompile"
	else
		return ret
	end

	src_tree['files'].each do |v|
		ext_name = File.extname(v)
		is_valid = false
		if ext_name.casecmp(".c") == 0  || ext_name.casecmp(".cpp") == 0 || ext_name.casecmp(".cc") == 0 
			if(type == "src_file")
				is_valid = true
			end
		elsif ext_name.casecmp(".h") == 0 || ext_name.casecmp(".hpp") == 0 
			if(type == "head_file")
				is_valid = true
			end
		end

		if is_valid
			v = v.gsub("/","\\")
			if !is_for_filter
				ret += "#{prefix_spaces}<#{tag} Include=\"#{v}\" />\n"	
			else
				ret += "#{prefix_spaces}<#{tag} Include=\"#{v}\" >\n"	
				ret += "#{prefix_spaces}  <Filter>#{new_parent_path_at_os}</Filter>\n"
				ret += "#{prefix_spaces}</#{tag}>\n"
			end			
		end

	end

	return ret
end

#根据变量获取对应值
def get_var_str(var_type, var, var_param, prefix_spaces, project_type, config)
	src_tree_type = %W(src_tree src_all_imp_file  src_all_head_file
	src_all_imp_file_for_filter src_all_head_file_for_filter
	src_all_dir_for_filter
	)
	if src_tree_type.index var_type
		ret = ""
		#p var_type
		#p prefix_spaces
		var.each do |v|
			#p v
			if var_param=="" || var_param==v['name']
				case var_type
				when "src_tree"
					tmp = get_src_tree_str(nil, v, project_type, prefix_spaces)
				when "src_all_imp_file"
					tmp = get_src_all_str(nil, v, project_type, prefix_spaces, "src_file", false)
				when "src_all_head_file"
					tmp = get_src_all_str(nil, v, project_type, prefix_spaces, "head_file", false)
				when "src_all_imp_file_for_filter"
					tmp = get_src_all_str(nil, v, project_type, prefix_spaces, "src_file", true)
				when "src_all_head_file_for_filter"
					tmp = get_src_all_str(nil, v, project_type, prefix_spaces, "head_file", true)
				when "src_all_dir_for_filter"
					tmp = get_src_all_str(nil, v, project_type, prefix_spaces, "dir", true)
				else
					return nil
				end		
				ret += tmp
			end
		end			
		#去掉第一行的缩进
		if ret.size>0
			ret = ret[prefix_spaces.size..-1]	
		end
	else
		case var_type
		when "str"
			ret = var
		when "macro"
			ret = get_macro_str(var, project_type, prefix_spaces)
		when "inc" #include
			#get_inc_str(var, project_type, prefix_spaces)
			ret = get_inc_str(var, project_type, prefix_spaces)
		when "share_lib"
			ret = get_share_lib_str(var, project_type, prefix_spaces)
		when "static_lib"
			ret = get_static_lib_str(var, project_type, prefix_spaces, config['lib_dirs'])					
		when "file"
			ret = File.open(var).read					
		else
			return nil
		end
	end	

	return ret
end

def make_project_file(outfile, template_file_path, project_type)
	template_file = File.open template_file_path
	line_no = 0
	template_file.each_line do |line|
		line_no+=1
		#寻找类似与  @{[config:platform]inc:inc_dirs}
		start_index =line.index('@{')
		if  start_index != nil
			prefix_spaces = ""
			if start_index != 0
				prefix_spaces = line[0..start_index-1]
			end
			# if line =~ /@{(\w+):(.*)}/
			# 	var_type = $1
			# 	var = $2
			# 	#puts $1
			# 	line = make_file_list(new_var, prefix_spaces)
			# end
			#puts prefix_spaces
			line = line.chomp.gsub(/@{([^}]*)}/) do |str|
				config_platform_str = ""
				key_value_str = $1
				if key_value_str.index("[")
					if key_value_str=~/\[(.*)\](.*)/
						config_platform_str = $1
						key_value_str = $2
					end
				end 
				tmp = config_platform_str.split(":")
				config_str = tmp[0]?tmp[0]:""
				platform_str = tmp[1]?tmp[1]:""
				config = find_config(config_str, platform_str)
				#pp config
				#exit

				tmp = key_value_str.split(":")
				var_type = tmp[0]?tmp[0]:""
				var = tmp[1]?tmp[1]:""
				var_param = ""
				if var=~ /.*\((.*)\)/
					var = $1
					var_param = $2
				end
				if config==nil
					exit_p "#{template_file_path}[#{line_no}]: error: config[#{config_str}:#{platform_str}] not find"
				end

				if var_type.downcase == "file"
					exit_p "#{template_file_path}[#{line_no}]: error: file[#{var}] is not exist!" if !File.exist?(var)	
				elsif var==""
					exit_p "#{template_file_path}[#{line_no}]: error: not have key"
				else
					exit_p "#{template_file_path}[#{line_no}]: error: key[#{var}]'s value is null" if config[var]==nil	
				end

				ret = get_var_str(var_type.downcase, config[var], var_param, prefix_spaces, project_type, config)
				if ret == nil
					exit_p "#{template_file_path}[#{line_no}]: error: unknow key type[#{var_type}]"
				end
				
				ret
			end
			#p line
			if line.strip() != ""
				outfile.puts line
			end
		else
			outfile.puts line			
		end
	end
end


#检查参数
if $options.build_config_file_path==nil || $options.project_template_file_path==nil || $options.project_type==nil
	exit_p("param error")
end

#处理所有普通输入参数
#num:当前处理的是第几个,  arg:当前处理的参数的名字(可能是文件,目录或其他)
#out_file:输出文件,已经打开 	其中如果输出文件名$options.out_file_name是nil那就是使用了终端$stdout
#error_file:错误输出文件,已经打开 其中如果错误输出文件名$options.error_file_name是nil那就是使用了终端$stderr
#另外out_file和error_file可以在each_file函数中传入,例CommonOptionParser.each_file($options, $stdout, $stderr) ...
# CommonOptionParser.each_arg() do |num, arg, out_file, error_file|
# 	#puts Benchmark.measure{
# 	in_file = nil
# 	if arg == $stdin
# 		in_file = arg
# 	elsif File.directory? arg
# 		out_file.puts "#{num}:#{arg} is directory"
# 		next
# 	else
# 		in_file = File.open arg
# 	end

# 	str = in_file.read(4)
# 	out_file.puts "#{num}:#{arg}[#{str}]"

# 	#}
# end

$all_config = YAML.load_file $options.build_config_file_path
#pp config
#project_template_file = File.open $options.project_template_file_path	
make_project_file(get_out_file(), $options.project_template_file_path, $options.project_type)