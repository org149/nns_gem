# encoding: UTF-8
require 'rubygems'
begin
  require 'bundler'
  Bundler::GemHelper.install_tasks
  require 'bundler/setup'
rescue LoadError
  puts 'You must `gem install bundler` and `bundle install` to run rake tasks'
end

require 'rake'
require 'rake/rdoctask'

require 'rspec/core'
require 'rspec/core/rake_task'

RSpec::Core::RakeTask.new(:spec)

task :default => :spec

Rake::RDocTask.new(:rdoc) do |rdoc|
  rdoc.rdoc_dir = 'rdoc'
  rdoc.title    = 'NNSearcher'
  rdoc.options << '--line-numbers' << '--inline-source'
  rdoc.rdoc_files.include('README.rdoc')
  rdoc.rdoc_files.include('lib/**/*.rb')
end

RSpec::Core::RakeTask.new(:rcov) do |t|
  t.rcov = true
  t.rcov_opts = %w[--rails]
  t.rcov_opts << "--exclude /gems/,/Library/,/usr/,spec,lib/tasks"
end

