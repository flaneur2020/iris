task :default => :run

task :run => :build do
  sh "bin/main"
end

task :build => [:link, :ctags]

mkdir_p 'bin'

cfiles = Dir['src/*.cpp']
hfiles = Dir['src/*.h']
ofiles = cfiles.map{|fn_c| 'bin/' + File.basename(fn_c).ext('o') }

cfiles.each do |fn_c|
  fn_o = 'bin/' + File.basename(fn_c).ext('o')
  file fn_o => [fn_c, *hfiles] do
    sh "g++ -c -Wall #{fn_c} -o #{fn_o}"
  end
end

task :link => ofiles do
  sh "g++ #{ofiles * ' '} -o bin/main"
end

task :clean do
  sh "rm -r bin/"
end

task :ctags do
  sh "ctags -R src"
end 
