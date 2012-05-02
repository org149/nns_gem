require 'spec_helper'
require 'csv'

describe NNSearcher do

  before( :all ) do
    @coords = []
    counter = 0
    zoom = 10000
    CSV.open( File.expand_path('../../support/cities.csv', __FILE__ ), "r", ';') do |row|
      counter += 1
      @coords << [ counter, row[4].to_i * zoom, row[3].to_i * zoom ]
    end
  end

  describe ".initialize" do

    it "should initialize without errors" do
      NNSearcher.new( @coords ).should_not == nil
    end

  end

  describe "#nearest_neighbour" do

    it "should find nearest coords" do
      nns = NNSearcher.new( @coords )
      coord = nns.nearest_neighbour( 45.39087, 50.12676 )
      puts coord.inspect
      coord.should_not == nil
    end

  end

end

