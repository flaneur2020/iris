task :run => :build do
  sh "bin/main"
end

task :build => :link

mkdir_p 'bin'

cfiles = Dir['src/*.c']
ofiles = cfiles.map{|fn_c| 'bin/' + File.basename(fn_c).ext('o') }

cfiles.each do |fn_c|
  fn_o = 'bin/' + File.basename(fn_c).ext('o')
  file fn_o => fn_c do
    sh "gcc -c #{fn_c} -o #{fn_o}"
  end
end

task :link => ofiles do
  sh "gcc #{ofiles * ' '} -o bin/main"
end
