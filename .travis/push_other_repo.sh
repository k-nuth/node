#!/bin/bash

echo "Travis branch: ${TRAVIS_BRANCH}"
echo "Travis tag: ${TRAVIS_TAG}"

if [[ ${TRAVIS_BRANCH} == ${TRAVIS_TAG} ]]; then
    export KTH_PUSH_BRANCH=master
else
    export KTH_PUSH_BRANCH=${TRAVIS_BRANCH}
fi
echo "Knuth branch: ${KTH_PUSH_BRANCH}"


# TODO(fernando): put in another place
export KTH_PUSH_ACCOUNT=kth

# ------------------------------------------------------
export KTH_PUSH_PROJECT=rpc

# body="{
#     \"accountName\": \"kth\",
#     \"projectSlug\": \"node\",
#     \"branch\": \"${KTH_PUSH_BRANCH}\",
#     \"environmentVariables\": {
#        \"SKIP_NUGET\": \"true\"
#     }
# }"


body="{
    \"accountName\": \"${KTH_PUSH_ACCOUNT}\",
    \"projectSlug\": \"${KTH_PUSH_PROJECT}\",
    \"branch\": \"${KTH_PUSH_BRANCH}\"
}"

curl -s -d "$body" -X POST \
    -H "Authorization: Bearer ${APPVEYOR_TOKEN}" \
    -H "Content-Type: application/json" \
    https://ci.appveyor.com/api/builds

body="{
    \"request\": {
    \"branch\":\"${KTH_PUSH_BRANCH}\",
    \"message\": \"Force by kth-node build: ${TRAVIS_BUILD_NUMBER}\"
}}"

curl -s -X POST \
   -H "Content-Type: application/json" \
   -H "Accept: application/json" \
   -H "Travis-API-Version: 3" \
   -H "Authorization: token ${TRAVIS_TOKEN}" \
   -d "$body" \
   https://api.travis-ci.org/repo/${KTH_PUSH_ACCOUNT}%2F${KTH_PUSH_PROJECT}/requests


# ------------------------------------------------------
export KTH_PUSH_PROJECT=kth-node-cint

# body="{
#     \"accountName\": \"kth\",
#     \"projectSlug\": \"node\",
#     \"branch\": \"${KTH_PUSH_BRANCH}\",
#     \"environmentVariables\": {
#        \"SKIP_NUGET\": \"true\"
#     }
# }"


body="{
    \"accountName\": \"${KTH_PUSH_ACCOUNT}\",
    \"projectSlug\": \"${KTH_PUSH_PROJECT}\",
    \"branch\": \"${KTH_PUSH_BRANCH}\"
}"

curl -s -d "$body" -X POST \
    -H "Authorization: Bearer ${APPVEYOR_TOKEN}" \
    -H "Content-Type: application/json" \
    https://ci.appveyor.com/api/builds

body="{
    \"request\": {
    \"branch\":\"${KTH_PUSH_BRANCH}\",
    \"message\": \"Force by kth-node build: ${TRAVIS_BUILD_NUMBER}\"
}}"

curl -s -X POST \
   -H "Content-Type: application/json" \
   -H "Accept: application/json" \
   -H "Travis-API-Version: 3" \
   -H "Authorization: token ${TRAVIS_TOKEN}" \
   -d "$body" \
   https://api.travis-ci.org/repo/${KTH_PUSH_ACCOUNT}%2F${KTH_PUSH_PROJECT}/requests




