\echo Use "CREATE EXTENSION phonenumber" to load this file. \quit

CREATE TYPE phonenumber_format AS ENUM ('e164', 'international', 'national', 'rfc3966');

CREATE OR REPLACE FUNCTION is_valid_number(IN phonenumber text)
RETURNS bool AS 'libphonenumber', 'is_valid_number'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION is_valid_number(IN phonenumber text,  IN region text)
RETURNS bool AS 'libphonenumber', 'is_valid_number_with_region'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION is_valid_number_for_region(IN phonenumber text, IN region text)
RETURNS bool AS 'libphonenumber', 'is_valid_number_for_region'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION format_number(IN phonenumber text, IN format phonenumber_format)
RETURNS text AS 'libphonenumber', 'format_number'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

CREATE OR REPLACE FUNCTION format_number(IN phonenumber text, IN region text, IN format phonenumber_format)
RETURNS text AS 'libphonenumber', 'format_number_with_region'
LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
