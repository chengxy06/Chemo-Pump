TARGET_NAME = "target"
#生成VS工程文件
def vs_generate_project(prj_dir,dest_file_name, project_template_name, config_yaml_file_path)
	tool_name = "vs"
	#project_file_name = "#{dest_file_name}.vcxproj"
	project_file_name = "#{TARGET_NAME}.vcxproj"
	filter_file_name = project_file_name +".filters"

	#通过生成配置生成最终工程文件
	sh "#{GENERATE_PROJECT_FILE_BIN} --project_template=#{prj_dir}/#{project_template_name} " +
				"--build_config=#{config_yaml_file_path} --project_type=#{tool_name} -o #{project_file_name}"
	#删除旧的工程文件,将新的工程文件移动到指定位置
	rm_rf "#{prj_dir}/#{project_file_name}"
	mv project_file_name,prj_dir

	#生成filters文件
	sh "#{GENERATE_PROJECT_FILE_BIN} --project_template=#{prj_dir}/filters_template " +
				"--build_config=#{config_yaml_file_path} --project_type=#{tool_name} -o #{filter_file_name}"
	#删除旧的filters,将新的文件移动到指定位置
	rm_rf "#{prj_dir}/#{filter_file_name}"
	mv filter_file_name,prj_dir
end
def vs_run_cmd(cmd)
	vs_cmd_tmp_file_name = "vs_cmd.bat"
	File.open vs_cmd_tmp_file_name,"w" do |bat_file|
		bat_file.puts "@call \"%VS140COMNTOOLS%\\vsvars32.bat\""
		bat_file.puts cmd
	end
	sh vs_cmd_tmp_file_name
	rm_rf vs_cmd_tmp_file_name
end
def vs_build(dest_file_name, config)
	vs_run_cmd "msbuild #{TARGET_NAME}.vcxproj /p:Configuration=#{config} /t:build /v:m /m"
end
def vs_clean(dest_file_name, config)
	vs_run_cmd "msbuild #{TARGET_NAME}.vcxproj /p:Configuration=#{config} /t:clean /v:m /m"	
end
def vs_install(dest_file_name, config, dest_dir)
end
$support_ide['vs'] = {:dir=>"build/VS", :generate_project=>method(:vs_generate_project), 
	:build=>method(:vs_build), :clean=>method(:vs_clean), :install=>method(:vs_install)}