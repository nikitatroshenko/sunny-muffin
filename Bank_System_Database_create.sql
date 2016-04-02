-- Created by Nikita



-- tables
-- Table: BANK_ACCOUNTS
CREATE TABLE BANK_ACCOUNTS (
    account_number integer  NOT NULL   PRIMARY KEY  AUTOINCREMENT,
    BANK_CLIENTS_id integer  NOT NULL,
    type text  NOT NULL,
    balance real  NOT NULL,
    FOREIGN KEY (BANK_CLIENTS_id) REFERENCES BANK_CLIENTS (id)
);

-- Table: BANK_CLIENTS
CREATE TABLE BANK_CLIENTS (
    id integer  NOT NULL   PRIMARY KEY  AUTOINCREMENT,
    is_individual boolean  NOT NULL,
    name text  NOT NULL,
    address text  NOT NULL
);

-- Table: BANK_CONFIG
CREATE TABLE BANK_CONFIG (
    configuration_property text  NOT NULL   PRIMARY KEY,
    value text  NOT NULL
);

-- Table: BANK_USERS
CREATE TABLE BANK_USERS (
    username text  NOT NULL   PRIMARY KEY,
    password text  NOT NULL,
    user_type text  NOT NULL
);





-- End of file.

