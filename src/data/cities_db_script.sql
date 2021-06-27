CREATE TABLE IF NOT EXISTS CITIES (
id TEXT,
    name TEXT,
    asciiname TEXT,
    alternativeNames TEXT,
    lat REAL,
    lon REAL,
    featureClass TEXT,
    featureCode TEXT,
    country TEXT,
    altCountry TEXT,
    adminCode TEXT,
    countrySubdivision TEXT,
    municipality TEXT,
    municipalitySubdivision TEXT,
    population TEXT,
    elevation TEXT,
    dem TEXT,
    tz TEXT,
    lastModified,
PRIMARY KEY(id)
);

.separator "\t"

.import cities1000.txt Cities

