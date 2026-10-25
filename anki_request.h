#pragma once
#include "curl/curl.h"
#include "third_party/json.hpp"
#include <iostream>
#include <string>

enum class ANKIMethods { NOTES_CREATE_MANY, DECKS_LIST, DECKS_CREATE };

nlohmann::json empty;
int anki_request(ANKIMethods method, nlohmann::json params = empty) {

  // JSON RPC Configuration
  nlohmann::json request;
  request["jsonrpc"] = "2.0";
  request["id"] = 1;

  switch (method) {
  case ANKIMethods::NOTES_CREATE_MANY:
    request["method"] = "notes.createMany";
    request["params"] = params;
    break;

  case ANKIMethods::DECKS_LIST:
    request["method"] = "decks.list";
    break;

  case ANKIMethods::DECKS_CREATE:
    request["method"] = "decks.create";
    request["params"] = params;
    break;
  }

  //=====================================================================
  // Sending the curl request
  //=====================================================================
  std::string request_str = request.dump();
  CURL *curl = curl_easy_init();

  curl_easy_setopt(curl, CURLOPT_URL, "http://127.0.0.1:8765/api/v1");

  curl_easy_setopt(curl, CURLOPT_HEADER, "Content-Type: application/json");
  curl_easy_setopt(curl, CURLOPT_POSTFIELDS, request_str.c_str());

#ifdef print_debug
  std::cout << request.dump(4);
#endif

  CURLcode result = curl_easy_perform(curl);
  if (result != CURLE_OK) {
    return 1;
  } else {
    return 0;
  }
}
