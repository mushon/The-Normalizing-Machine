# MP  namespace
MP = @MP = {} unless MP?

# config shared by all environments
MP.config =
  version: "0.0.1",
  env: "dev" # available envs: dev, test, prod
