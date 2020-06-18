#include <iostream>
#include <fstream>
#include <string>
#include <cctype>
#include <vector>
#include <sstream>
#include <bits/stdc++.h>
#include <algorithm>
#include <stdio.h>
#include <stdlib.h>

#include "ProbTable.hpp"

using namespace std;

// Helper funciton to alphabetically sort the words in the vocab
bool alphabetical(string a, string b) {
    return a < b;
}

// Helper to call sort
vector<string> alphabeticallySort(vector<string> vocab) {
    std::vector<string>::iterator ip;

    sort(vocab.begin(), vocab.end(), alphabetical);

    // Remove duplicates from vocab
    ip = std::unique(vocab.begin(), vocab.end());
    vocab.resize(std::distance(vocab.begin(),ip));

    return vocab;
}

// Removes punctuation and digits as well as caps
string removePunc(string line) {
    int len = line.size();
    for (int i = 0; i < len; i++) {
        if (ispunct(line[i]) || isdigit(line[i])) {
            if(!(i >= len - 3)) {
                line.erase(i--, 1);
                len = line.size();
            }
        }
    }
    // Convert all of the capitals to lowercase
    for (int i = 0; i < len; i++) {
        if (line[i] >= 65 && line[i] <= 92) {
            line[i] = line[i] + 32;
        }
    }
    return line;
}

int main(int argc, char *argv[]) {

    string line, strippedLine;

    if (argc != 3) {
        cout << "Usage: ./sentiment <trainingSet.txt> <testSet.txt>" << endl;
        return 1;
    }

    // Open the training set
    ifstream TrainingFile(argv[1]);
    ofstream stripped("strippedTrain.txt");
 
    // Form the vocabulary, the set of all of the words in the training set
    vector<string> vocab;
    int trainingLines = 0;
    
    while(getline(TrainingFile, line)) {

        // Strip the file
        line = removePunc(line);

        // Put each word in each line into the vocabulary
        istringstream iss(line);
        string word;
        while(iss >> word) {
            // Don't add the classification variable to the vocab
            if (word.compare("0") && word.compare("1")) {
                vocab.push_back(word);
            }
        }

        stripped << line << endl;
        trainingLines++;
    }

    stripped.close();
    TrainingFile.close();
    
    // Alphabetize vocabulary
    vocab = alphabeticallySort(vocab);
    

    /* ******************************************************************* *
     * After we have our vocabulary, we need to turn the training set and  *
     * test set into features                                              *
     * ******************************************************************* */

    // Declare a 2D array the size of the training file
    int trainFeatures[trainingLines][vocab.size() + 1];
    // Set all of the values to 1 row by row
    for (int i = 0; i < trainingLines; i++) {
        for (int j = 0; j < vocab.size() + 1; j++) {
            trainFeatures[i][j] = 0;
        }
    }

    // Read from the stripped file and add to the features list
    ifstream strippedTrainFile("strippedTrain.txt");
    int i = 0;
    while(getline(strippedTrainFile, line)) {
        // Get the class variable
        int classVar = line[line.size() - 3] - 48;

        // Add features to the feature table
        istringstream iss(line);
        string word;
        while(iss >> word) {
            for (int j = 0; j < vocab.size(); j++) {
                if (!word.compare(vocab[j])) {
                    trainFeatures[i][j] = 1;
                }
            }
        }

        // Set the class variable
        trainFeatures[i][vocab.size()] = classVar;
        i++;
    }
    
    strippedTrainFile.close();
    
    /* ******************************************************************* *
     * Strip and create a 2D feature array for the test set                *
     * ******************************************************************* */
    
    // Open the test set
    ifstream testFile(argv[2]);
    ofstream strippedTest("strippedTest.txt");
 
    // Number of lines in the test set
    int testingLines = 0;
    
    while(getline(testFile, line)) {

        // Strip the file
        line = removePunc(line);

        strippedTest << line << endl;
        testingLines++;
    }

    strippedTest.close();
    testFile.close();

    // Declare a 2D array the size of the training file
    int testFeatures[testingLines][vocab.size() + 1];

    // Set all of the values to 1 row by row
    for (int i = 0; i < testingLines; i++) {
        for (int j = 0; j < vocab.size() + 1; j++) {
            testFeatures[i][j] = 0;
        }
    }

    // Read from the stripped file and add to the features list
    ifstream strippedTestFile("strippedTest.txt");
    i = 0;
    while(getline(strippedTestFile, line)) {
        // Get the class variable
        int classVar = line[line.size() - 3] - 48;

        // Add features to the feature table
        istringstream iss(line);
        string word;
        while(iss >> word) {
            for (int j = 0; j < vocab.size(); j++) {
                if (!word.compare(vocab[j])) {
                    testFeatures[i][j] = 1;
                }
            }
        }

        // Set the class variable
        testFeatures[i][vocab.size()] = classVar;
        i++;
    }
    
    strippedTestFile.close();

    /* ******************************************************************* *
     * Output preprocessed files                                           *
     * ******************************************************************* */

    // Training set features
    ofstream preprocessed_train("preprocessed_train.txt");
    for (int i = 0; i < vocab.size(); i++) {
        preprocessed_train << vocab[i] + ",";
    }
    // Output the class label
    preprocessed_train << "classlabel" << endl;

    for (int i = 0; i < trainingLines; i++) {
        for (int j = 0; j < vocab.size() + 1; j++) {
            preprocessed_train << trainFeatures[i][j];
        }
        preprocessed_train << endl;
    }
    
    preprocessed_train.close();
   
    // Test set features
    ofstream preprocessed_test("preprocessed_test.txt");
    for (int i = 0; i < vocab.size(); i++) {
        preprocessed_test << vocab[i] + ",";
    }
    // Output the class label
    preprocessed_test << "classlabel" << endl;

    for (int i = 0; i < testingLines; i++) {
        for (int j = 0; j < vocab.size() + 1; j++) {
            preprocessed_test << testFeatures[i][j];
        }
        preprocessed_test << endl;
    }
    
    preprocessed_test.close();

    /* ******************************************************************* *
     * Begin the Classification training                                   *
     * ******************************************************************* */
    float probBE = 0;
    float probWE = 0;
    float numRecordsBE = 0;
    float numRecordsWE = 0;

    // Determine the number of records in training data where classification
    // is true or false
    for (int i = 0; i < trainingLines; i++) {
        if(trainFeatures[i][vocab.size()] == 1) {
            numRecordsBE++;
        } else {
            numRecordsWE++;
        }
    }

    // Compute the probability of classification variable
    probBE = numRecordsBE/trainingLines;
    probWE = numRecordsWE/trainingLines;

    // Compute the probability of the each of the training sets features
    // Create a vector of ProbTable objects
    vector<ProbTable> parameters;

    // For each of the features in the vocab create a ProbTable object and 
    // populate the probParam array
    for (int i = 0; i < vocab.size(); i++) {
        // Create a new ProbTable object
        ProbTable table(vocab[i]);

        // Calculate each of the table entry params
        int numWithTT = 0;
        int numWithTF = 0;
        int numWithFT = 0;
        int numWithFF = 0;

        int location = 0;

        // Find the number with TT, TF, FT, FF
        // For each line 
        for (int j = 0; j < trainingLines; j++) {
            // If the feature and classification variable are both true
            if (trainFeatures[j][i] == 1 && trainFeatures[j][vocab.size()] == 1) {
                numWithTT++;
            } else if (trainFeatures[j][i] == 1 && trainFeatures[j][vocab.size()] == 0) {
                numWithTF++;
            } else if (trainFeatures[j][i] == 0 && trainFeatures[j][vocab.size()] == 1) {
                numWithFT++;
            } else {
                numWithFF++;
            }
        }


        // Probably have to work in logspace here
        // TODO
        // Add each element to the table object
        table.calcParams(numWithTT, numRecordsBE, 0);
        table.calcParams(numWithTF, numRecordsWE, 1);
        table.calcParams(numWithFT, numRecordsBE, 2);
        table.calcParams(numWithFF, numRecordsWE, 3);

        // Push the table onto the vector for each feature
        parameters.push_back(table);
    }

    /*
    for (int i = 0; i < parameters.size(); i++) {
        cout << parameters[i].getParam(0) <<  ", " << parameters[i].getParam(1) <<  ", " << parameters[i].getParam(2) <<  ", " << parameters[i].getParam(3) << endl;
    }
    */

    /* ******************************************************************* *
     * Intialize Classification of test set using testFeatures             *
     * ******************************************************************* */
    // Currently ignoring words that we didnt train on

    // Cnvert the probBE and probWE to log space
    probBE = log(probBE);
    probWE = log(probWE);
    
    int totalGuesses = 0;
    int numCorrect = 0;

    cout << "Line\tGuess\tActual" << endl;

    for (int i = 0; i < testingLines; i++) {

        // For each line
        long double bestSum = probBE;
        long double worstSum = probWE;

        for (int j = 0; j < vocab.size(); j++) {
            // For each word in sentence add (since in log space) prob in prob parameters
            // prob(classification=true) + sumof(currentWord=currentValue|classification=true)
            
            // Compute BE
            if (testFeatures[i][j] == 1) {
                bestSum = bestSum + parameters[j].getParam(0);    
            } else {
                bestSum = bestSum + parameters[j].getParam(2);    
            }

            // Compute WE
            if (testFeatures[i][j] == 0) {
                worstSum = worstSum + parameters[j].getParam(3);    
            } else {
                worstSum = worstSum + parameters[j].getParam(1);    
            }    
        }
        
        cout << i + 1 << ":\t";

        // Output the best param per line
        if (bestSum > worstSum) {
            cout << "1\t" << testFeatures[i][vocab.size()] << endl;
            if (testFeatures[i][vocab.size()] == 1) {
                numCorrect++;
            }
        } else { 
            cout << "0\t" << testFeatures[i][vocab.size()] << endl;
            if (testFeatures[i][vocab.size()] == 0) {
                numCorrect++;
            }
        }

        // Increment Guesses
        totalGuesses++;
    }
    
    cout << ((float) numCorrect/ (float) totalGuesses) * 100.00  << "%" << " accuracy" << endl;

    return 0;
}

