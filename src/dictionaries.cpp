#include "dictionaries.h"
#include "utils.h"
#include <QCoreApplication>
#include <QDir>
#include <QLibraryInfo>
#include <QString>
#include <QStringList>

QString DICTIONARY_FILE_SUFFIX = ".bdic";

Dictionaries::Dictionaries(QObject *parent) : QObject(parent) {
  setParent(parent);
}

Dictionaries::~Dictionaries() { this->deleteLater(); }

QString Dictionaries::GetDictionaryPath() {
  QString dict_path;

  // the environment variable takes precedence on all platforms
  if (qEnvironmentVariableIsSet("QTWEBENGINE_DICTIONARIES_PATH")) {
    dict_path = Utils::GetEnvironmentVar("QTWEBENGINE_DICTIONARIES_PATH");
    return dict_path;
  }

  QString dict_dir = "qtwebengine_dictionaries";

  // inside appdata dir /usr/share/appname
  QStringList dataLocations = QStandardPaths::standardLocations(QStandardPaths::GenericDataLocation);

  QString appdata_path;
  for (const QString &path : dataLocations) {
      if (path == "/usr/share") {
          appdata_path = path;
          break;
      }
  }

  if (appdata_path.isEmpty()) {
      appdata_path = dataLocations.filter("/usr/share").first();
  }
  dict_path =
      QString("%1/%2/%3")
          .arg(appdata_path,
               QApplication::applicationName(), dict_dir);
  qDebug()<< dict_path;
  if (QDir(dict_path).exists()) {
    return dict_path;
  }

  // next look relative to the executable
  dict_path =
      QCoreApplication::applicationDirPath() + QDir::separator() + dict_dir;
  if (QDir(dict_path).exists()) {
    return dict_path;
  }

  // inside the installed Qt directories
  dict_path = QLibraryInfo::location(QLibraryInfo::DataPath) +
              QDir::separator() + dict_dir;
  if (QDir(dict_path).exists()) {
    return dict_path;
  }

  return QString();
}

QStringList Dictionaries::GetDictionaries() {
  QStringList dictionaries;
  QString dict_path = GetDictionaryPath();
  if (dict_path.isEmpty()) {
    return dictionaries;
  }
  QDir dictDir(dict_path);
  if (dictDir.exists()) {
    QStringList filters;
    // Look for all *.bdic files.
    filters << "*" + DICTIONARY_FILE_SUFFIX;
    dictDir.setNameFilters(filters);
    QStringList dictionary_files = dictDir.entryList();
    foreach (QString file, dictionary_files) {
      QFileInfo fileInfo(file);
      QString dname = fileInfo.baseName();
      dictionaries.append(dname);
    }
  }
  return dictionaries;
}
