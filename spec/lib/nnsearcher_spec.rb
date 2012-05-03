require 'spec_helper'
require 'csv'

describe NNSearcher do

  before( :all ) do
    @coords = []
    counter = 0
    zoom = 10000
    CSV.open( File.expand_path('../../support/cities.csv', __FILE__ ), "r", ';') do |row|
      counter += 1
      @coords << [ counter, (row[4].to_f * zoom).to_i, (row[3].to_f * zoom).to_i ]
    end
  end

  describe ".initialize" do

    it "should initialize without errors" do
      lambda{
        NNSearcher.new( @coords )
      }.should_not raise_error
    end

  end

  describe "#nearest_neighbour" do

    before(:all) do
      @nns = NNSearcher.new( @coords )
    end

    context "standart cities search" do

      it "should find nearest coords" do # Волгоград
        @nns.nearest_neighbour( 44.4338, 48.7317 ).should == 204 
      end

      it "should find nearest coords" do # Москва / МГУ
        @nns.nearest_neighbour( 37.5872, 55.7479 ).should == 690
      end

      it "should find nearest coords" do # Екатерингбург
        @nns.nearest_neighbour( 60.6412, 56.8570 ).should == 320 
      end

      it "should find nearest coords" do # Питер
        @nns.nearest_neighbour( 30.4070, 59.9341 ).should == 950 
      end

      it "should find nearest coords" do # Саратов
        @nns.nearest_neighbour( 45.9180, 51.5731 ).should == 953
      end

    end

    context "complicated cities search" do

      it "should find nearest coords" do # Камышин
        @nns.nearest_neighbour( 45.3815, 50.1147 ).should == 435 
      end

      it "should find nearest coords" do # Раменское
        @nns.nearest_neighbour( 38.2465, 55.5699 ).should == 922 
      end

      it "should find nearest coords" do # Энгельс
        @nns.nearest_neighbour( 46.1528, 51.4660 ).should == 1279 
      end

      it "should find nearest coords" do # Нерюнгри
        @nns.nearest_neighbour( 124.6501, 56.6728 ).should == 726 
      end

      it "should find nearest coords" do # Уфа
        @nns.nearest_neighbour( 55.8705, 54.7136 ).should == 1173 
      end

      it "should find nearest coords" do # Пермь
        @nns.nearest_neighbour( 56.2171, 57.9904 ).should == 854 
      end

      it "should find nearest coords" do # Н.Новгород
        @nns.nearest_neighbour( 44.0092, 56.3299 ).should == 741 
      end

      it "should find nearest coords" do # Краснодар
        @nns.nearest_neighbour( 38.9806, 45.0421 ).should == 540 
      end

    end

  end

end

