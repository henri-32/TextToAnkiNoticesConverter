#include "anki_request.h"
#include "third_party/args.h"
#include "third_party/json.hpp"
#include <fstream>
#include <iostream>
#include <string>

nlohmann::json basic_note(std::string deck) {
  nlohmann::json note;

  note["noteType"] = nlohmann::json::object();
  note["noteType"]["name"] = "Basic";
  note["deck"] = nlohmann::json::object();
  note["deck"]["name"] = deck;
  note["fields"] = nlohmann::json::object();
  note["fields"]["Front"] = "";
  note["fields"]["Back"] = "";

  return note;
};

int main(int argc, char *argv[]) {
  // ===================================================================
  // Configure Argument parser
  // ===================================================================
  args::ArgParser parser{"Convert .md or .txt files to anki notes.\n \
		Usage: \n \
		anki_converter -f <Filename> -d <Anki deck> ",
                         "1.0"};

  parser.option("file f");
  parser.option("deck d");
  parser.parse(argc, argv);

  if (parser.value("file") == "") {
    std::cerr << "You need to enter a valid filename";
    return 1;
  }
  if (parser.value("deck") == "") {
    std::cerr << "You need to enter a valid anki deckname";
    return 1;
  }

  // ===================================================================
  // Setup the working containers needed to deserialize the file into json anki
  // notes
  // ===================================================================
  struct qa_pair {
    std::string question;
    std::string answer;
    void clear() {
      answer.clear();
      question.clear();
    }
  } next_pair;

  struct NotePairs {
    std::vector<qa_pair> string_pairs;
    std::vector<nlohmann::json> note_jsons;
  } notePairs;

  bool in_question = false;
  bool in_answer = false;

  //======================================================================
  // deserialize the file into json anki notes
  //======================================================================
  std::ifstream filestream{parser.value("file")};
  std::string line;
  while (std::getline(filestream, line)) {

    // Marker for beginning of questions
    if (line.starts_with("Q:")) {
      in_question = true;
    }

    if (in_question) {
      // Marker for end of questions
      while (line.find("?") == std::string::npos) {
        line += '\n';
        next_pair.question.append(line);
        std::getline(filestream, line);
        continue;
      }
      line += '\n';
      next_pair.question.append(line);
      in_question = false;
      continue;
    }

    // Marker for beginning of answers
    if (line.starts_with("A:")) {
      in_answer = true;
    }
    if (in_answer) {
      // Marker for end of answers
      while (line.find("!") == std::string::npos) {
        line += '\n';
        next_pair.answer.append(line);
        std::getline(filestream, line);
        continue;
      }
      line += '\n';
      next_pair.answer.append(line);
      in_answer = false;

      next_pair.answer.erase(next_pair.answer.find('!'));
	  next_pair.answer += '\n';

      // write this pair of parsed QA to the storage and clear for the next
      // lines
      notePairs.string_pairs.push_back(next_pair);
	  #ifdef print_debug
      std::cout << next_pair.question << "\n";
      std::cout << next_pair.answer << "\n";
	  #endif
      next_pair.clear();
    }
  }

  //======================================================================
  // Configure the note objects and transfer the string pairs to the json
  // objects
  //======================================================================

  // Get a basic configured note (json) compliant to the anki rpc api
  auto note = basic_note(parser.value("deck"));

  // Write the string pairs into the note container with HTML preformat
  for (size_t i = 0; i < notePairs.string_pairs.size(); ++i) {
    note["fields"]["Front"] = "<pre>" + notePairs.string_pairs.at(i).question + "</pre>";
    note["fields"]["Back"] = "<pre>" + notePairs.string_pairs.at(i).answer + "</pre>";
    notePairs.note_jsons.push_back(note);
  }

  //======================================================================
  // Configure the RPC parameter object and transfer the notes into them
  //======================================================================
  nlohmann::json params = nlohmann::json::object();
  params["notes"] = nlohmann::json::array();
  params["atomic"] = true;

  // Write the notes from the working container into the RPC params
  for (size_t i = 0; i < notePairs.note_jsons.size(); i++) {
    params["notes"].push_back(notePairs.note_jsons[i]);
  }

  //======================================================================
  // Call the Anki RPC API
  //======================================================================
  int returncode = anki_request(ANKIMethods::NOTES_CREATE_MANY, params);
  return returncode;
}
