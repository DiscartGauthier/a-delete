cmake_minimum_required(VERSION 3.10)

#Nom du projet
project(Serveur)

#Spécifier le standard C++
set (CMAKE_CXX_STANDARD 11)
set (CMAKE_CXX_STANDARD_REQUIRED True)

# Ajouter les fichiers sources automatiquement
file(GLOB SOURCES "src/*.cpp")

#Ajouter l'exécutable 
add_executable(${PROJECT_NAME} ${SOURCE})

#Lien avec la bibliothèque pthread
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)
