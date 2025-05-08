// test/tests_main.cpp
#include <unity.h>
#include <Arduino.h>

// Déclaration des fonctions qui regroupent les tests
extern void run_quadrature_encoder_tests();
extern void run_encoder_manager_tests();
extern void run_event_bus_tests();
extern void run_input_router_tests();
extern void run_midi_buffer_tests();
extern void run_process_buttons_tests();
extern void run_process_encoders_tests();
extern void run_profile_manager_tests();
extern void run_scheduler_tests();

void setup() {
    UNITY_BEGIN();
    
    // Exécuter tous les groupes de tests
    run_quadrature_encoder_tests();
    run_encoder_manager_tests();
    run_event_bus_tests();
    run_input_router_tests();
    run_midi_buffer_tests();
    run_process_buttons_tests();
    run_process_encoders_tests();
    run_profile_manager_tests();
    run_scheduler_tests();
    
    UNITY_END();
}

void loop() {
    // Vide - requis pour l'architecture Arduino
}
