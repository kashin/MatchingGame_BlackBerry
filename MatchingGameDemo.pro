APP_NAME = MatchingGameDemo

CONFIG += qt warn_on cascades10
LIBS += -lbbsystem

include(config.pri)

TRANSLATIONS = \
    $${TARGET}_en_GB.ts \
    $${TARGET}_ru.ts \
    $${TARGET}.ts
