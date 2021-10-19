#include <gtest/gtest.h>

#include "config.h"

class Config_UnitTests : public ::testing::Test
{
    
};

TEST_F(Config_UnitTests, FailOnConfigDefaults) {
    config config;
    EXPECT_FALSE(config.validate());
}
