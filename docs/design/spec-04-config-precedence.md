# Configuration Precedence

Configuration resolution follows command-line arguments first, explicit configuration files second, environment defaults third, and built-in defaults last.

Every resolved setting should be attributable to one source. Invalid high-priority values fail clearly instead of silently falling back to a lower-priority value.