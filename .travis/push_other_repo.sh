#!/bin/bash

echo "Travis branch: ${TRAVIS_BRANCH}"
echo "Travis tag: ${TRAVIS_TAG}"

if [[ ${TRAVIS_BRANCH} == ${TRAVIS_TAG} ]]; then
    export BITPRIM_PUSH_BRANCH=master
else
    export BITPRIM_PUSH_BRANCH=${TRAVIS_BRANCH}
fi
echo "Bitprim branch: ${BITPRIM_PUSH_BRANCH}"


# TODO(fernando): put in another place
export BITPRIM_PUSH_ACCOUNT=bitprim

# ------------------------------------------------------
export BITPRIM_PUSH_PROJECT=bitprim-rpc

# body="{
#     \"accountName\": \"bitprim\",
#     \"projectSlug\": \"bitprim-node\",
#     \"branch\": \"${BITPRIM_PUSH_BRANCH}\",
#     \"environmentVariables\": {
#        \"SKIP_NUGET\": \"true\"
#     }
# }"


body="{
    \"accountName\": \"${BITPRIM_PUSH_ACCOUNT}\",
    \"projectSlug\": \"${BITPRIM_PUSH_PROJECT}\",
    \"branch\": \"${BITPRIM_PUSH_BRANCH}\"
}"

curl -s -d "$body" -X POST \
    -H "Authorization: Bearer ${APPVEYOR_TOKEN}" \
    -H "Content-Type: application/json" \
    https://ci.appveyor.com/api/builds

body="{
    \"request\": {
    \"branch\":\"${BITPRIM_PUSH_BRANCH}\",
    \"message\": \"Force by bitprim-node build: ${TRAVIS_BUILD_NUMBER}\"
}}"

curl -s -X POST \
   -H "Content-Type: application/json" \
   -H "Accept: application/json" \
   -H "Travis-API-Version: 3" \
   -H "Authorization: token ${TRAVIS_TOKEN}" \
   -d "$body" \
   https://api.travis-ci.org/repo/${BITPRIM_PUSH_ACCOUNT}%2F${BITPRIM_PUSH_PROJECT}/requests


# ------------------------------------------------------
export BITPRIM_PUSH_PROJECT=bitprim-node-cint

# body="{
#     \"accountName\": \"bitprim\",
#     \"projectSlug\": \"bitprim-node\",
#     \"branch\": \"${BITPRIM_PUSH_BRANCH}\",
#     \"environmentVariables\": {
#        \"SKIP_NUGET\": \"true\"
#     }
# }"


body="{
    \"accountName\": \"${BITPRIM_PUSH_ACCOUNT}\",
    \"projectSlug\": \"${BITPRIM_PUSH_PROJECT}\",
    \"branch\": \"${BITPRIM_PUSH_BRANCH}\"
}"

curl -s -d "$body" -X POST \
    -H "Authorization: Bearer ${APPVEYOR_TOKEN}" \
    -H "Content-Type: application/json" \
    https://ci.appveyor.com/api/builds

body="{
    \"request\": {
    \"branch\":\"${BITPRIM_PUSH_BRANCH}\",
    \"message\": \"Force by bitprim-node build: ${TRAVIS_BUILD_NUMBER}\"
}}"

curl -s -X POST \
   -H "Content-Type: application/json" \
   -H "Accept: application/json" \
   -H "Travis-API-Version: 3" \
   -H "Authorization: token ${TRAVIS_TOKEN}" \
   -d "$body" \
   https://api.travis-ci.org/repo/${BITPRIM_PUSH_ACCOUNT}%2F${BITPRIM_PUSH_PROJECT}/requests




