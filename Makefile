all: sentiment

sentiment: Sentiment.cpp ProbTable.cpp ProbTable.hpp
	g++ -o sentiment Sentiment.cpp ProbTable.cpp

clean:
	rm -rf sentiment results.txt strippedTest.txt strippedTrain.txt preprocessed_train.txt preprocessed_test.txt
