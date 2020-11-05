#pragma once
#include <memory>

#include "common/definitions.h"
#include "data/alignment.h"
#include <algorithm>
#include <unordered_set>


#include <unordered_map>
namespace marian {
// A Trie node
struct Trie
{
    // true when node is a leaf node
    bool isLeaf;

    //list of words that would complete the constraint from current node
    std::vector<Word> finalIds;
    // each node stores a map to its child nodes
    std::unordered_map<Word, marian::Trie*> map;
};

// Function that returns a new Trie node
inline Trie* getNewTrieNode()
{
    Trie* node = new Trie;
    node->isLeaf = false;

    return node;
}

// Iterative function to insert a string in Trie.
    inline  void insert(Trie*& head, std::vector<Word> tokenSequence)
{
    if (head == nullptr)
        head = getNewTrieNode();

    // start from root node
    Trie* curr = head;
    int i=0;
    for  (auto token:tokenSequence)
    {

        // create a new node if path doesn't exists
        if (curr->map.find(token) == curr->map.end())
            curr->map[token] = getNewTrieNode();

        //penultimate word in a sequence
        if (i==tokenSequence.size()-1){
            curr->finalIds.push_back(tokenSequence.back());
        }

        // go to next node
        curr = curr->map[token];


        i++;
    }

    // mark current node as leaf
    curr->isLeaf = true;
}

// returns true if given node has any children
    inline  bool haveChildren(Trie const* curr)
{
    // don't use (curr->map).size() to check for children

    for (auto it : curr->map)
        if (it.second != nullptr)
            return true;

    return false;
}

// Recursive function to delete a string in Trie.
    inline  bool deletion(Trie*& curr, std::vector<Word> tokenSequence)
{
    // return if Trie is empty
    if (curr == nullptr)
        return false;

    // if we have not reached the end of the string
    int i=0;
    for  (auto token:tokenSequence)
    {
        // recur for the node corresponding to next character in
        // the string and if it returns true, delete current node
        // (if it is non-leaf)
        if (curr != nullptr &&  curr->map.find(token) != curr->map.end() &&
            deletion(curr->map[token], tokenSequence) && curr->isLeaf == false)
        {
            if (!haveChildren(curr))
            {
                delete curr;;
                curr = nullptr;
                return true;
            }
            else {
                return false;
            }
        }
        i++;
    }

    // if we have reached the end of the string
    if (curr->isLeaf)
    {
        // if current node is a leaf node and don't have any children
        if (!haveChildren(curr))
        {
            delete curr;; // delete current node
            curr = nullptr;
            return true; // delete non-leaf parent nodes
        }

            // if current node is a leaf node and have children
        else
        {
            // mark current node as non-leaf node (DON'T DELETE IT)
            curr->isLeaf = false;
            return false;	   // don't delete its parent nodes
        }
    }

    return false;
}

// Iterative function to search a string in Trie. It returns true
// if the string is found in the Trie, else it returns false
    inline  bool search(Trie* head, std::vector<Word> tokenSequence)
{
    // return false if Trie is empty
    if (head == nullptr)
        return false;

    Trie* curr = head;
    for  (auto token:tokenSequence)
    {
        // go to next node
        curr = curr->map[token];

        // if string is invalid (reached end of path in Trie)
        if (curr == nullptr)
            return false;
    }

    // if current node is a leaf and we have reached the
    // end of the string, return true
    return curr->isLeaf;
}

    inline  Trie* copy(Trie* orig){
;
    Trie *newTrie=getNewTrieNode();
    return newTrie;
}
/*
// Memory efficient Trie Implementation in C++ using Map
int main()
{
    Trie* head = nullptr;

    insert(head, "hello");
    cout << search(head, "hello") << " ";   	// print 1

    insert(head, "helloworld");
    cout << search(head, "helloworld") << " ";  // print 1

    cout << search(head, "helll") << " ";   	// print 0 (Not present)

    insert(head, "hell");
    cout << search(head, "hell") << " ";		// print 1

    insert(head, "h");
    cout << search(head, "h") << endl;  		// print 1 + newline

    deletion(head, "hello");
    cout << search(head, "hello") << " ";   	// print 0 (hello deleted)
    cout << search(head, "helloworld") << " ";  // print 1
    cout << search(head, "hell") << endl;   	// print 1 + newline

    deletion(head, "h");
    cout << search(head, "h") << " ";   		// print 0 (h deleted)
    cout << search(head, "hell") << " ";		// print 1
    cout << search(head, "helloworld") << endl; // print 1 + newline

    deletion(head, "helloworld");
    cout << search(head, "helloworld") << " ";  // print 0
    cout << search(head, "hell") << " ";		// print 1

    deletion(head, "hell");
    cout << search(head, "hell") << endl;   	// print 0 + newline

    if (head == nullptr)
        cout << "Trie empty!!\n";   			// Trie is empty now

    cout << search(head, "hell");   			// print 0

    return 0;
}*/


// one single (partial or full) hypothesis in beam search
// key elements:
//  - the word that this hyp ends with
//  - the aggregate score up to and including the word
//  - back pointer to previous hypothesis for traceback


    class Hypothesis {
public:
  typedef IPtr<Hypothesis> PtrType;

private:
  // Constructors are private, use Hypothesis::New(...)

  Hypothesis() : prevHyp_(nullptr), prevBeamHypIdx_(0), word_(Word::ZERO), pathScore_(0.0) {}

  Hypothesis(const PtrType prevHyp,
             Word word,
             size_t prevBeamHypIdx, // beam-hyp index that this hypothesis originated from
             float pathScore)
      : prevHyp_(prevHyp), prevBeamHypIdx_(prevBeamHypIdx), word_(word), pathScore_(pathScore) {}

public:
    std::vector<Trie *> constraintTrieStates;
    Trie * constraintTrieRoot;

    // Use this whenever creating a pointer to MemoryPiece
 template <class ...Args>
 static PtrType New(Args&& ...args) {
   return PtrType(new Hypothesis(std::forward<Args>(args)...));
 }

  const PtrType getPrevHyp() const { return prevHyp_; }

  Word getWord() const { return word_; }

  size_t getPrevStateIndex() const { return prevBeamHypIdx_; }

  float getPathScore() const { return pathScore_; }

  const std::vector<float>& getScoreBreakdown() { return scoreBreakdown_; }
  void setScoreBreakdown(const std::vector<float>& scoreBreakdown) { scoreBreakdown_ = scoreBreakdown; }

  float getLastWordScore() {
    if (prevHyp_) {
      return pathScore_ - prevHyp_->getPathScore();
    } else {
      return pathScore_;
    }
  }

  const std::vector<float>& getAlignment() { return alignment_; }
  void setAlignment(const std::vector<float>& align) { alignment_ = align; };

  // trace back paths referenced from this hypothesis
  Words tracebackWords() {
    Words targetWords;
    for(auto hyp = this; hyp->getPrevHyp(); hyp = hyp->getPrevHyp().get()) {
      targetWords.push_back(hyp->getWord());
    }
    std::reverse(targetWords.begin(), targetWords.end());
    return targetWords;
  }

  // calculate word-level scores for each target word by de-aggregating the path score
  std::vector<float> tracebackWordScores() {
    std::vector<float> scores;
    // traverse hypotheses backward
    for(auto hyp = this; hyp->getPrevHyp(); hyp = hyp->getPrevHyp().get()) {
      // a path score is a cumulative score including scores from all preceding hypotheses (words),
      // so calculate a word-level score by subtracting the previous path score from the current path score
      auto prevPathScore = hyp->getPrevHyp() ? hyp->getPrevHyp().get()->pathScore_ : 0.f;
      scores.push_back(hyp->pathScore_ - prevPathScore);
    }
    std::reverse(scores.begin(), scores.end());
    return scores;
  }

  // get soft alignments [t][s] -> P(s|t) for each target word starting from the hyp one
  typedef data::SoftAlignment SoftAlignment;
  SoftAlignment tracebackAlignment() {
    SoftAlignment align;
    for(auto hyp = this; hyp->getPrevHyp(); hyp = hyp->getPrevHyp().get()) {
      align.push_back(hyp->getAlignment());
    }
    std::reverse(align.begin(), align.end());
    return align;  // [t][s] -> P(s|t)
  }

private:
  const PtrType prevHyp_;
  const size_t prevBeamHypIdx_;
  const Word word_;
  const float pathScore_;

  std::vector<float> scoreBreakdown_; // [num scorers]
  std::vector<float> alignment_;

  ENABLE_INTRUSIVE_PTR(Hypothesis)
};

typedef std::vector<IPtr<Hypothesis>> Beam;                // Beam = vector [beamSize] of hypotheses
typedef std::vector<Beam> Beams;                          // Beams = vector [batchDim] of vector [beamSize] of hypotheses
typedef std::tuple<Words, IPtr<Hypothesis>, float> Result; // (word ids for hyp, hyp, normalized sentence score for hyp)
typedef std::vector<Result> NBestList;                    // sorted vector of (word ids, hyp, sent score) tuples
}  // namespace marian
