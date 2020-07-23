#YelpReviewSentiment
This program uses Naive Bayes to determine whether on not a Yelp Review had positive or negative sentiment. The program trains itself using the training set and then makes a judgement about each of the Yelp reviews in the test set. 

I have a make file included so to compile use the make command 

then to run the program use the command:

    ./sentiment trainingSet.txt testSet.txt > results.txt

After running this command there will be the preprocessed files, stripped files, and the
results file. The stripped files were used to make my reading of the data easier to read
and debug.
