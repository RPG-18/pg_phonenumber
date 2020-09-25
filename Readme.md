# pg_phonenumber

PostgreSQL extension for for parsing, formatting, and validating international phone numbers by [Google's libphonenumber](https://github.com/google/libphonenumber). 

## Usage

| Function                                             | Return Type | Example                                                                     | Result         |
|:-----------------------------------------------------|:------------|:----------------------------------------------------------------------------|:---------------|
| is_valid_number(number text[, region text])          | bool        |is_valid_number('+1-212-555-0100'), is_valid_number('+1-212-555-0100', 'US') | true           |
| is_valid_number_for_region(number text, region text) | bool        |is_valid_number_for_region('+1-212-555-0100', 'RU')                          | false          |
| format_number(number text,[, region text], format phonenumber_format) | text | format_number('+1-212-555-0100', 'national')                       | (212) 555-0100 |
