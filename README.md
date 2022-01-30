# WordleSolver
Gives you the "best" word to guess in every wordle scenario.
NOTE: return output to the solver in a string composed of ".", "g", and "y" (. = grey, g = green, y = yellow).

The process (as of 1/30/2022):
1. All the words of the given length are put into a separate file (if it does not already exist).
2. Weights are developed, where the weight of a certain letter at a certain position in the word represents how many words have that specific letter in that position.
3. The highest-weight guess is chosen and given to the user.
4. The result that is given by wordle is parsed, and all words that it could no longer be are removed. The remaining words are re-weighted based on the new weights.
5. Repeat steps 3 and 4 until the correct word is returned.
