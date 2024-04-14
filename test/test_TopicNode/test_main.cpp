#include <cstdio>
#include <catch2/catch_test_macros.hpp>

#include <tito/TopicNode.h>

char buffer[55];
int result;

TEST_CASE( "TopicNode.buildTopic" ) {
  // topic = tito::TopicNode(nullptr, "name");

  // printf("TopicNode size: %lu\n", sizeof(tito::TopicNode));

  // printf("nullptr size: %s\n", sizeof(topic));
  // printf("nullptr.name: %s\n", topic.getText());

  tito::TopicNode root(nullptr, "root");
  result = root.buildTopic(buffer, 55, "state");
  printf("buffer: |%s| (%ld - %d)\n", buffer, root.getTotalLen(), 0);

  REQUIRE( strcmp(buffer, "root/state") == 0 );
  REQUIRE( result == 10 );

  result = root.buildTopic(buffer, 10, "something");
  printf("buffer: |%s| (%ld - %d)\n", buffer, root.getTotalLen(), 0);
  REQUIRE( strcmp(buffer, "root/state") == 0 );
  REQUIRE( result == 0 );

  tito::TopicNode branch(&root, "branch");
  result = branch.buildTopic(buffer, 55, "state");
  printf("buffer: |%s| (%ld - %ld)\n", buffer, branch.getTotalLen(), root.getTotalLen());
  REQUIRE( strcmp(buffer, "root/branch/state") == 0 );

  REQUIRE( result == (4 + 1 + 6 + 1 + 5) );

  tito::TopicNode deep(&branch, "deep");
  result = deep.buildTopic(buffer, 55, "state");
  printf("buffer: |%s| (%ld - %ld - %ld)\n", buffer, deep.getTotalLen(), branch.getTotalLen(), root.getTotalLen());
  REQUIRE( strcmp(buffer, "root/branch/deep/state") == 0 );

  REQUIRE( result == (4 + 1 + 6 + 1 + 4 + 1 + 5) );
}

TEST_CASE( "TopicNode.topicMatches" ) {
  tito::TopicNode root(nullptr, "root");
  tito::TopicNode brnc(&root, "branch");
  tito::TopicNode deep(&brnc, "deep");

  REQUIRE(  root.topicMatches("root", 4) );
  REQUIRE( !root.topicMatches("toot", 4) );
  REQUIRE(  brnc.topicMatches("root/branch", 11) );
  REQUIRE( !brnc.topicMatches("root/pranch", 11) );
  REQUIRE(  deep.topicMatches("root/branch/deep", 16) );
  REQUIRE( !deep.topicMatches("root/pranch/deep", 16) );
  REQUIRE( !deep.topicMatches("root/branch/jeep", 16) );
  REQUIRE( !deep.topicMatches("root/branch/not",  15) );
}
