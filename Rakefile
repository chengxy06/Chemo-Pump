require 'YAML'
require 'pathname'

#rake -T   --  list all task
#rake xxx debug=1   -- at debug mode
#
#verbose(false) #禁止输出sh本身的命令
SELF_ABSOLUTE_PATH = Pathname.new(__FILE__).realpath
SELF_DIR = File.dirname(SELF_ABSOLUTE_PATH)
GENERATE_PROJECT_FILE_BIN = "ruby -S generate_project_file.rb"
BUILD_CONFIG_BIN = "ruby build/config/build_config.rb"
CREATE_TEST_BIN = "ruby tools/create_test_file/create_test_file.rb"
CREATE_RUN_ALL_TEST_SUITE_BIN = "ruby tools/bin/create_run_all_test_suite.rb"
PC_LINT_BIN = "Lint-nt"
GENERATE_LANG_BIN = "ruby tools/bin/generate_lang.rb"
GENERATE_FONT_BIN = "ruby tools/bin/generate_font.rb"
GENERATE_IMAGE_BIN = "ruby tools/bin/generate_image.rb"

DEFAULT_CONFIG = "release"
DEFAULT_COMPILER = "iar"
DEFAULT_INSTALL_DIR = "."
#TEST_BUILD_CONFIG_BIN = "build/config/test_build_config.rb"

ENV['PATH']="#{SELF_DIR}/tools/bin;#{ENV['PATH']}"
#判断当前操作系统
def os_family  
  case RUBY_PLATFORM  
    when /ix/i, /ux/i, /gnu/i,  
         /sysv/i, /solaris/i,  
         /sunos/i, /bsd/i  
      "unix"  
    when /win/i, /ming/i  
      "windows"  
    else  
      "other"  
  end  
end  

#生成工程文件
def generate_project(tool_name, tool_handlers)
	config_yaml_file_path = "#{tool_name}_build_config.yaml"
	sh "#{BUILD_CONFIG_BIN} --tool=#{tool_name} -o #{config_yaml_file_path}"
	all_config = YAML.load_file config_yaml_file_path
	$all_config = all_config
	build_config = all_config[1] #获取一个编译配置
	case build_config['dest_file_type']
	when "bin"
		project_template_name = "bin_project_template"
	when "share"
		project_template_name = "share_project_template"
	when "static"
		project_template_name = "static_project_template"
	else
		fail "unknow dest file type: #{build_config['dest_file_type']}"
	end		
	#call XX_generate_project
	tool_handlers[:generate_project].call(tool_handlers[:dir], build_config['dest_file_name'], 
		project_template_name, config_yaml_file_path)

	rm_rf config_yaml_file_path if ENV['debug']!="1"
end

#获取目标配置
def get_dest_config
	if $dest_config == nil
		if File.exist?("dest_config.yaml")
			$dest_config = YAML.load_file "dest_config.yaml"
		else
			fail "no config file, need rake config first"
		end
	end

	return $dest_config
end

#修改软件名字
def set_software_name_at_file(dest_file_path, name)
	file_content = File.read dest_file_path
	dest_file = File.open(dest_file_path+".tmp", "w")
	file_content.each_line do |line|
		if line =~ /#define SOFTWARE_NAME "(.*)"/
			line = "#define SOFTWARE_NAME \"#{name}\"\n"
		end
		dest_file.write line
	end
	dest_file.close
	rm dest_file_path
	mv dest_file_path+".tmp",dest_file_path
end

#返回在SVN上的版本号和对应日期
def get_info_at_svn()
	ret_str = `svn info`
	#puts ret_str
	rev = 0
	rev_date = ""

	ret_str.each_line do |line|
		#puts line
		if line =~ /^Last Changed Rev:\s*(\d+)/
			rev = $1
		elsif line =~ /^Last Changed Date: (.+)/
			rev_date = $1
		end
	end

	if rev == 0
		fail "can not get revision at SVN"
	end
	return rev, rev_date
end
#更新内部版本号到指定文件
def update_internal_version(version_file_path, internal_version)
	infile_name = version_file_path
	outfile_name = version_file_path+"_new"
	infile = File.new infile_name
	outfile = File.new outfile_name,"w"

	infile.each_line do |line|
		if line.index "#define INTERNAL_VERSION"
			line = "#define INTERNAL_VERSION #{internal_version}\n"
		end
		outfile.print line
	end
	infile.close()
	outfile.close()
	File.delete(infile_name)
	File.rename(outfile_name, infile_name)
	return internal_version
end

#从指定文件获取版本号,返回软件名,四个版本号,如"ssz_micro_sys_example",'1','1','4','456'
def get_version_info(version_file_path)
	software_name = "null"
	major_version = "0"
	minor_version = "0"
	revision_version = "0"
	internal_version = "0"
	infile = File.new version_file_path
	infile.each_line do |line|
		#p line
		if line =~ /#define INTERNAL_VERSION\s+(\d+)/
			internal_version = $1
		elsif line =~ /#define MAJOR_VERSION\s+(\d+)/
			major_version = $1
		elsif line =~ /#define MINOR_VERSION\s+(\d+)/
			minor_version = $1
		elsif line =~ /#define REVISION_VERSION\s+(\d+)/
			revision_version = $1
		elsif line =~ /#define SOFTWARE_NAME\s+\"(.*)\"/
			software_name = $1
		end
	end
	infile.close()
	return software_name,major_version,minor_version, revision_version, internal_version
end

#检查是否有更改的文件,询问是否继续
def check_if_has_changed_file(is_need_ask= true)
	#检查是否有修改过的代码
	ret_str = `svn status -q`
	changed_files = []
	ret_str.each_line do |line|
		if line =~ /\bversion.c\b/
			
		else
			#puts line
			changed_files<<line
		end
	end
	if changed_files.size > 0
		if is_need_ask
			puts "The code directory have changed files:"
			changed_files.each do |v|
				puts v
			end
			puts "Do you want to continue? Press yes to continue or abort."
			str = STDIN.gets
			if str == "yes\n"
				
			else
				puts "Abort"
				exit -1
			end
		else
			fail "The code directory have changed files."
		end
	end
end
#生成发布目录,用于发布固件
def make_and_return_publish_dir(dest_dir, compiler_str, config_name)
	software_name,major_version,minor_version, revision_version, internal_version = 
			get_version_info("./source/version.c")
	#puts get_version_info("#{prj_dir}/source/version.c")
	rev, rev_date = get_info_at_svn()

	#创建存放目录
	config_str = ""
	if compiler_str != "iar"
		config_str +="_"+compiler_str
	end
	if config_name != "Release"
		config_str += "_"+config_name
	end

	new_dest_dir = dest_dir + "/#{software_name}"+config_str+
		"_V#{major_version}.#{minor_version}.#{revision_version}.#{internal_version}"
	if Dir.exist? new_dest_dir
		rm_rf new_dest_dir
	end
	mkdir(new_dest_dir)	

	#生成description.txt文件
	File.open("#{new_dest_dir}/description.txt","w") do |out|
		out.puts "Dir Name: #{File.basename(new_dest_dir)}"
		out.puts "SVN Version and Date: #{internal_version}, #{rev_date}"
		out.puts "Release Date: #{Time.now}"
	end

	return new_dest_dir
end
#加载支持的编译环境
$support_ide ={}
Dir["*_define.rb"].each do |v|
	begin
		load v
	rescue Exception => e
		fail "load #{v} fail!\n#{e}"
	end
end

desc "update software name, create project file"
task :first_init do
	all_build_config =YAML.load  `#{BUILD_CONFIG_BIN} --tool=vs --no_src `
	dest_file_name = all_build_config[0]['dest_file_name']
	puts "generate simulator sln"
	cp "simulator/sln_template", "simulator/#{dest_file_name}.sln"
	puts "update version.h"
	set_software_name_at_file("source/version.h", dest_file_name)
	Rake::Task[:project].invoke()
end

desc "generate project file for IDE"
task :project,[:IDE] do |t, args|
	if args.IDE == nil
		$support_ide.each_pair() do |key, value|
			puts "generate #{key} project file"
			generate_project(key, value)
		end
	else
		ide = args.IDE.downcase
		fail "unsupport IDE: #{args.IDE}" if $support_ide[ide]==nil
		puts "generate #{args.IDE} project file"
		generate_project(ide, $support_ide[ide])		
	end
end

desc "config[<compiler>|<IDE>, <config>, <platform>] to genarate config file, the platform can ignore"
task :config,[:compiler_or_IDE, :build_config, :platform] do |t, args|
	#set the config,compiler,platform
	build_config_str = args.build_config
	build_config_str = DEFAULT_CONFIG if build_config_str == nil
	compiler_str = args.compiler_or_IDE
	compiler_str = DEFAULT_COMPILER if compiler_str==nil
	platform_str = args.platform
	if platform_str == nil
		if os_family() == "windows"
			platform_str = "win"
		else
			platform_str = "unix"
		end
	end

	config_map = {"build_config"=>build_config_str, "is_use_ide"=>false,
		 "compiler_or_IDE"=>compiler_str.downcase(), "platform"=>platform_str, "dir"=>"build/Rake/"}
	is_build_use_ide = false
	#check if specify IDE or compiler
	$support_ide.each_pair() do |key, value|
		if key== compiler_str.downcase()
			Rake::Task[:project].invoke(key)
			is_build_use_ide = true
			config_map["is_use_ide"] = true
			config_map["compiler_or_IDE"] = key
			config_map["dir"] = value[:dir]
			config_map['dest_file_name'] = $all_config[1]['dest_file_name']
			$all_config.each do |v|
				if v['config'].casecmp(build_config_str) == 0
					config_map['build_config'] = v['config']
					build_config_str = v['config']
					break
				end
			end
			break
		end
	end	

	if !is_build_use_ide
		puts "Genarate rake config"
		config_yaml_file_path = config_map["dir"]+"/build_config.yaml"
		sh "#{BUILD_CONFIG_BIN} --tool=Rake --compiler=#{compiler_str} >#{config_yaml_file_path} "	
		all_config = YAML.load_file config_yaml_file_path
		build_config = all_config[1] #获取一个编译配置
		config_map['dest_file_name'] = build_config['dest_file_name']
		File.open config_map["dir"]+"/dest_config.yaml","w" do |f|
			f.puts config_map.to_yaml()
		end
	end

	if is_build_use_ide
		puts "Generate target config: #{compiler_str}_#{build_config_str} "
	else
		puts "Generate target config: #{compiler_str}_#{build_config_str}_#{platform_str} "
	end
	File.open "dest_config.yaml","w" do |f|
		f.puts config_map.to_yaml()
	end
end

desc "clean"
task :clean do
	dest_config = get_dest_config()
	cd dest_config["dir"]
	if dest_config["is_use_ide"]
		ide_handlers = $support_ide[dest_config['compiler_or_IDE']]
		ide_handlers[:clean].call(dest_config['dest_file_name'], dest_config['build_config'])
	else
		sh "rake clean"
	end
	cd SELF_DIR
end

desc "install to <dir>"
task :install,[:dest_dir] do |t, args|
	if args.dest_dir
		dest_dir = args.dest_dir
	else
		dest_dir = DEFAULT_INSTALL_DIR
	end
	dest_config = get_dest_config()
	publish_dir = File.expand_path( make_and_return_publish_dir(dest_dir, dest_config['compiler_or_IDE'], 
		dest_config['build_config']) )
	cd dest_config["dir"]
	if dest_config["is_use_ide"]
		ide_handlers = $support_ide[dest_config['compiler_or_IDE']]
		ide_handlers[:install].call(dest_config['dest_file_name'], dest_config['build_config'], publish_dir)
	else
		sh "rake install[#{publish_dir}]"
	end
	cd SELF_DIR
	puts "install to #{publish_dir}"
end

desc "make target"
task :default do
	dest_config = get_dest_config()
	cd dest_config["dir"]
	if dest_config["is_use_ide"]
		ide_handlers = $support_ide[dest_config['compiler_or_IDE']]
		ide_handlers[:build].call(dest_config['dest_file_name'], dest_config['build_config'])
	else
		sh "rake"
	end
	cd SELF_DIR
end

desc "update version to file for compile"
task :update_version_to_file do
	internal_version, rev_date =  get_info_at_svn()
	puts "update version.c with Internal Version:#{internal_version}"
	update_internal_version("source/version.c", internal_version)
end

desc "make debug bin"
task :debug=> [:update_version_to_file] do
	all_build_config = YAML.load  `#{BUILD_CONFIG_BIN} --tool=iar --no_src `
	dest_file_name = all_build_config[0]['dest_file_name']
	ide_handlers = $support_ide["iar"]

	cd ide_handlers[:dir]
	ide_handlers[:build].call(dest_file_name, "Debug")
	
	cd SELF_DIR
	publish_dir = File.expand_path( make_and_return_publish_dir(DEFAULT_INSTALL_DIR, "iar", "Debug") )
	cd ide_handlers[:dir]
	ide_handlers[:install].call(dest_file_name, "Debug", publish_dir)
	cd SELF_DIR
end

desc "make release bin"
task :release=> [:update_version_to_file]  do
	check_if_has_changed_file()

	all_build_config = YAML.load `#{BUILD_CONFIG_BIN} --tool=iar --no_src `
	dest_file_name = all_build_config[0]['dest_file_name']
	ide_handlers = $support_ide["iar"]
	
	cd ide_handlers[:dir]
	ide_handlers[:build].call(dest_file_name, "Release")

	cd SELF_DIR
	publish_dir = File.expand_path( make_and_return_publish_dir(DEFAULT_INSTALL_DIR, "iar", "Release") )
	cd ide_handlers[:dir]
	ide_handlers[:install].call(dest_file_name, "Release", publish_dir)
	cd SELF_DIR
end

desc "make simulator"
task :sim=> [:update_version_to_file] do
	all_build_config =YAML.load  `#{BUILD_CONFIG_BIN} --tool=vs --no_src `
	dest_file_name = all_build_config[0]['dest_file_name']
	ide_handlers = $support_ide["vs"]

	cd ide_handlers[:dir]
	ide_handlers[:build].call(dest_file_name, "Debug")
	
	cd SELF_DIR
	publish_dir = File.expand_path( make_and_return_publish_dir(DEFAULT_INSTALL_DIR, "vs", "Debug") )
	cd ide_handlers[:dir]
	ide_handlers[:install].call(dest_file_name, "Debug", publish_dir)
	cd SELF_DIR
end

desc "make debug bin and simulator"
task :all do
	Rake::Task[:debug].invoke
	Rake::Task[:sim].invoke 
end

desc "create test file for the specify file"
task :create_test,[:src_file] do |t, args|
	all_build_config =YAML.load  `#{BUILD_CONFIG_BIN} --tool=IAR --simple_src `
	srcs = all_build_config[0]['src_files']
	src_path = nil
	srcs.each do |file_path|
		if File.basename(file_path).casecmp(File.basename(args.src_file))==0
			src_path = File.expand_path(file_path, "build/config/")
			break
		end
	end
	if src_path==nil
		puts "not find the file: #{args.src_file}" 
	else
		file_name = File.basename(args.src_file, ".*")+".c"
		file_name.insert(0,"test_")
		sh "#{CREATE_TEST_BIN} #{src_path}" 
		mv  file_name, "test/#{file_name}"
		sh "#{CREATE_RUN_ALL_TEST_SUITE_BIN} test/test_*.c -o test/run_all_test_suite.c"
		Rake::Task[:project].invoke
	end
end

desc "create test suite"
task :create_test_suite do
	sh "#{CREATE_RUN_ALL_TEST_SUITE_BIN} test/test_*.c test/test_*.cpp -o test/run_all_test_suite.c"
end

desc "lint"
task :lint,[:result_file] do |t, args|
	#将工程目录添加到环境变量, lint需要用
	ENV['PROJ_PATH']=Dir.pwd()

	all_build_config =YAML.load  `#{BUILD_CONFIG_BIN} --tool=IAR --simple_src `
	release_config = all_build_config[1]
	names_file = File.open("src_file.lnt", "w")
	all_build_config[0]['src_files'].each do |file_path|
		if File.extname(file_path)!=".c" && File.extname(file_path)!=".cpp"
			next
		end
		if file_path.include?("bsp/") || file_path.include?("test/")
			next
		end
		src_path = File.expand_path(file_path, "build/config/")
		names_file.puts src_path
	end
	names_file.close
	proj_opt_file = File.open("custom_project_option.lnt", "w")
	release_config['inc_dirs'].each do |path|
		src_path = File.expand_path(path, "build/config/")
		#src_path.gsub!("/","\\")
		proj_opt_file.puts "-i\"#{src_path}\"  "
	end
	release_config['macros'].each do |macro|
		proj_opt_file.puts "-d#{macro}"
	end
	proj_opt_file.close

	rest_file = ""
	if args.result_file
		rest_file = "-os(#{args.result_file})"
	end	
	cmd = "#{PC_LINT_BIN} +v #{rest_file} -itools/PC-lint std.lnt custom_project_option.lnt src_file.lnt "
	puts cmd
	system cmd
end

desc "lint one file, it need read some config created by lint, so need lint run once"
task :lint_file,[:file_path,:result_file] do |t, args|
	#将工程目录添加到环境变量, lint需要用
	ENV['PROJ_PATH']=Dir.pwd()
	rest_file = ""
	if args.result_file
		rest_file = "-os(#{args.result_file})"
	end

	src_path = nil
	all_build_config =YAML.load  `#{BUILD_CONFIG_BIN} --tool=IAR --simple_src `
	all_build_config[0]['src_files'].each do |file_path|
		if File.basename(file_path).casecmp(File.basename(args.file_path))==0
			src_path = File.expand_path(file_path, "build/config/")
			break
		end		
	end

	#puts args.file_path
	cmd = "#{PC_LINT_BIN} +v #{rest_file} -itools/PC-lint std.lnt custom_project_option.lnt -u #{src_path} "
	puts cmd
	system cmd
end

desc "generate string"
task :string do
	sh "#{GENERATE_LANG_BIN} resource/lang.xlsx"
	mv ["generated_string_data.h","generated_string_id.h"], "source/ui_common/resource/"
end

desc "generate font"
task :font do
	sh "#{GENERATE_FONT_BIN} -b font_bmp -d font_code resource/font_str_define.ini"
	rm_rf "source/ui_common/resource/font" 
	mkdir "source/ui_common/resource/font"
	cp "generated_font_data.h", "source/ui_common/resource/"
	cp Dir["font_code/*.c"], "source/ui_common/resource/font/"
	rm_rf ["generated_font_data.h", "font_bmp","font_code"] if ENV["debug"] != "1"
end

desc "generate image"
task :image do
	sh "#{GENERATE_IMAGE_BIN} resource/image/*.bmp -d img_code"
	rm_rf "source/ui_common/resource/image" 
	mkdir "source/ui_common/resource/image"
	cp "generated_image_data.h", "source/ui_common/resource/"
	cp "generated_image_id.h", "source/ui_common/resource/"
	cp Dir["img_code/*.c"], "source/ui_common/resource/image/"
	rm_rf ["generated_image_data.h", "generated_image_id.h", "img_code"] if ENV["debug"] != "1"	
end

desc "generate resource, update project file"
task :resource => [:string,:font,:image] do
	Rake::Task[:project].invoke
	#Rake::Task[:test].invoke
end 
