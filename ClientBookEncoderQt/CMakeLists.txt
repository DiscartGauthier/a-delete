cmake_minimum_required(VERSION 3.10)

# Nom du projet
project(ApplicBooker)

# Spécifier le standard C++
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED True)

# Trouver les modules QT nécessaires
find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

# Inclure le module FindPkgConfig
find_package(PkgConfig REQUIRED)
pkg_check_modules(MYSQL REQUIRED mysqlclient)

# Inclure le répertoire include
include_directories(include ${MYSQL_INCLUDE_DIRS})

# Ajouter les fichiers sources automatiquement
file(GLOB SOURCES "src/*.cpp")

link_directories(${MYSQL_LIBRARY_DIRS})

# Ajouter l'exécutable 
add_executable(${PROJECT_NAME} ${SOURCES})

# Lier les bibliothèques Qt nécessaires
target_link_libraries(${PROJECT_NAME} Qt5::Core Qt5::Gui Qt5::Widgets ${MYSQL_LIBRARIES})

