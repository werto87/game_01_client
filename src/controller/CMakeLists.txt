 target_sources(${TARGET_NAME} PRIVATE 
 ${CMAKE_CURRENT_LIST_DIR}/stateMachine.hxx
 ${CMAKE_CURRENT_LIST_DIR}/commonEvent.hxx
 ${CMAKE_CURRENT_LIST_DIR}/playTheGame.hxx
 ${CMAKE_CURRENT_LIST_DIR}/playTheGameState.hxx
 ${CMAKE_CURRENT_LIST_DIR}/playTheGameAction.hxx
 ${CMAKE_CURRENT_LIST_DIR}/loginStateMachine.hxx
 ${CMAKE_CURRENT_LIST_DIR}/makeGameMachine.hxx
 ${CMAKE_CURRENT_LIST_DIR}/stateMachineData.hxx
 ${CMAKE_CURRENT_LIST_DIR}/loginStateMachineAction.hxx
 ${CMAKE_CURRENT_LIST_DIR}/makeGameMachineAction.hxx
 ${CMAKE_CURRENT_LIST_DIR}/sendObject.hxx
 ${CMAKE_CURRENT_LIST_DIR}/loginStateMachineState.hxx
 ${CMAKE_CURRENT_LIST_DIR}/makeGameMachineState.hxx
 stateMachineData.cxx
 )