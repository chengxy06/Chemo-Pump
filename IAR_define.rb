require "digest/md5"

#生成IAR工程文件
def iar_generate_project(prj_dir, dest_file_name, project_template_name, config_yaml_file_path)
	tool_name = "iar"
	project_file_name = dest_file_name+".ewp"
	eww_file_name = dest_file_name +".eww"

	#通过生成配置生成最终工程文件
	sh "#{GENERATE_PROJECT_FILE_BIN} --project_template=#{prj_dir}/#{project_template_name} " +
				"--build_config=#{config_yaml_file_path} --project_type=#{tool_name} -o #{project_file_name}"
	#删除旧的工程文件,将新的工程文件移动到指定位置
	rm_rf "#{prj_dir}/#{project_file_name}"
	mv project_file_name,prj_dir

	#生成eww文件
	sh "#{GENERATE_PROJECT_FILE_BIN} --project_template=#{prj_dir}/eww_template " +
				"--build_config=#{config_yaml_file_path} --project_type=#{tool_name} -o #{eww_file_name}"
	#删除旧的eww,将新的文件移动到指定位置
	rm_rf "#{prj_dir}/#{eww_file_name}"
	mv eww_file_name,prj_dir

end
def iar_build(dest_file_name, config)
	sh "iarbuild #{dest_file_name}.ewp -make #{config}"	
end
def iar_clean(dest_file_name, config)
	sh "iarbuild #{dest_file_name}.ewp -clean #{config}"		
end
def iar_install(dest_file_name, config, dest_dir)

	#复制生成的文件到存放目录并且添加只读属性
	out_file_paths = []
	out_file_paths << "#{config}/Exe/#{dest_file_name}.bin" 
	out_file_paths << "#{config}/Exe/#{dest_file_name}.out" 
	out_file_paths.each do |path|
		cp path, dest_dir
		system("attrib +R #{dest_dir}/#{File.basename(path)}")
		File.open "#{dest_dir}/#{File.basename(path)}.md5","w" do |out|
			out.print Digest::MD5.hexdigest(File.read(path,mode:"rb"))
			out.print " *#{File.basename(path)}"
		end
	end

end
$support_ide['iar'] = {:dir=>"build/IAR", :generate_project=>method(:iar_generate_project), 
	:build=>method(:iar_build), :clean=>method(:iar_clean), :install=>method(:iar_install)}