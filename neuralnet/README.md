#TODO: Finish this readme properly lol

As of right now, it takes two line-delimited numbers and prints out the output as a single number. It rounds that number to 0 or 1 in order to make a "guess".

Can easily be trained to learn AND and OR functions, and can learn other simple logic operations with enough training input/target values.

Trainer.cpp can be modified to create other sets of input. Pipe it's output into a file called "training", run the neuralnet, and say "train" to have
the neuralnet read and learn from the training file. Currently set up to produce XOR operation data.

Tell the neuralnet "score" to see the neuralnet's current score.
