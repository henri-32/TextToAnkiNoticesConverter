#include "curl/curl.h"
#include "third_party/json.hpp"
#include <fstream>
#include <iostream>
#include <string>

int main(int argc, char *argv[]) {

  if (argc == 1) {
    std::cout << "Convert .md or .t files to anki notices " << '\n'
              << "Usage:" << '\n'
              << " anki_converter -<Filename> -<Anki Decks>" << '\n';
    return 0;
  }

  if (argv[1] == nullptr || argc < 2)
    return 1;

  std::string file_to_convert = argv[1];
  std::cout << file_to_convert << '\n';

  std::ifstream filestream{file_to_convert};
  std::string line;

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

  while (std::getline(filestream, line)) {

    if (line.starts_with("Q:")) {
      in_question = true;
    }

    if (in_question) {
      while (line.find("?") == std::string::npos) {
        next_pair.question.append(line);
        std::getline(filestream, line);
        continue;
      }
      // Hier müsste jetzt ein Fragezeichen drin sein
      next_pair.question.append(line);
      in_question = false;
      continue;
    }
    // Nach der vollständigen Frage die nächste Zeile lesen

    if (line.starts_with("A:")) {
      in_answer = true;
    }
    if (in_answer) {
      while (line.find("!") == std::string::npos) {
        next_pair.answer.append(line);
        std::getline(filestream, line);
        continue;
      }
      // Sollte ! enthalten
      next_pair.answer.append(line);
      next_pair.answer.erase(next_pair.answer.find('!'));
      in_answer = false;
      notePairs.string_pairs.push_back(next_pair);
      next_pair.clear();
    }
  }

  nlohmann::json note;
  note["noteType"] = nlohmann::json::object();
  note["noteType"]["name"] = "Basic";
  note["deck"] = nlohmann::json::object();
  note["deck"]["name"] = "Henri könnte ein richtiger Programmierer werden";
  note["fields"] = nlohmann::json::object();
  note["fields"]["Front"] = "";
  note["fields"]["Back"] = "";

  for (size_t i = 0; i < notePairs.string_pairs.size(); ++i) {
    note["fields"]["Front"] = notePairs.string_pairs.at(i).question;
    note["fields"]["Back"] = notePairs.string_pairs.at(i).answer;
    notePairs.note_jsons.push_back(note);
  }

  std::string rpc_method = "notes.createMany";

  nlohmann::json request;
  request["jsonrpc"] = "2.0";
  request["id"] = 1;
  request["method"] = rpc_method;

  if (rpc_method == "notes.createMany") {
    request["params"]["notes"] = nlohmann::json::array();
    request["params"]["atomic"] = true;

    for (size_t i = 0; i < notePairs.note_jsons.size(); i++) {
      request["params"]["notes"].push_back(notePairs.note_jsons[i]);
    }
  }
  std::cout << "WARN size of string pairs " << notePairs.string_pairs.size()
            << '\n';
  std::cout << "WARN size of jsonpairs "
            << std::to_string(notePairs.note_jsons.size()) << '\n';
  std::cout << "COMPLETE REQUEST" << request.dump() << '\n';

  CURL *curl = curl_easy_init();

  std::string xxx =
      R"({"jsonrpc": "2.0", "id":1, "method":"decks.list", "params":{}})";

  std::string request_str = request.dump();
  curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:8765/api/v1");

  curl_easy_setopt(curl, CURLOPT_HEADER, "Content-Type: application/json");
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_str.c_str());

  CURLcode result = curl_easy_perform(curl);

  if (result != CURLE_OK) {
    std::cerr << curl_easy_strerror(result) << '\n';
  }

  curl_easy_cleanup(curl);
  return result == CURLE_OK ? 0 : 1;
}
