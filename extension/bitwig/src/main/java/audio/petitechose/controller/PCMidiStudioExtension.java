package audio.petitechose.controller;

import com.bitwig.extension.controller.ControllerExtension;
import com.bitwig.extension.controller.api.*;

/**
 * Main implementation class for the PC MIDI Studio Controller
 * This class handles all the controller logic and MIDI communication
 */
public class PCMidiStudioExtension extends ControllerExtension {
   
   // Core components
   private ControllerHost host;
   private Transport transport;
   private MidiIn midiIn;
   private MidiOut midiOut;
   private HardwareSurface hardwareSurface;
   
   // Bitwig API objects
   private TrackBank trackBank;
   private CursorTrack cursorTrack;
   private CursorDevice cursorDevice;
   private PinnableCursorDevice primaryDevice;
   
   // Configuration
   private static final int TRACK_COUNT = 8;
   private static final int SCENE_COUNT = 8;
   private static final int SEND_COUNT = 2;
   
   // MIDI CC mappings (customize these for your controller)
   private static final int CC_FADER_1 = 1;
   private static final int CC_KNOB_1 = 21;
   private static final int CC_BUTTON_1 = 41;
   
   protected PCMidiStudioExtension(
         PCMidiStudioExtensionDefinition definition,
         ControllerHost host) {
      super(definition, host);
      this.host = host;
   }
   
   @Override
   public void init() {
      // Initialize the extension
      host.println("PC MIDI Studio: Initializing...");
      
      // Setup MIDI ports
      setupMidiPorts();
      
      // Create hardware surface
      setupHardwareSurface();
      
      // Setup Bitwig API objects
      setupBitwigAPI();
      
      // Configure MIDI callbacks
      setupMidiCallbacks();
      
      // Initialize controller state
      initializeController();
      
      host.println("PC MIDI Studio: Initialization complete!");
   }
   
   private void setupMidiPorts() {
      // Get MIDI ports
      midiIn = getMidiInPort(0);
      midiOut = getMidiOutPort(0);
      
      // Configure MIDI input
      midiIn.setMidiCallback(this::onMidi);
      midiIn.setSysexCallback(this::onSysex);
      
      // Create note input for keyboard/pads if needed
      // Uncomment and customize if your controller has keys/pads
      // NoteInput noteInput = midiIn.createNoteInput("PC MIDI Studio", "80????", "90????");
      // noteInput.setShouldConsumeEvents(false);
   }
   
   private void setupHardwareSurface() {
      // Create hardware surface for visual representation
      hardwareSurface = host.createHardwareSurface();
      hardwareSurface.setPhysicalSize(300, 200); // Adjust to match your controller
      
      // Example: Create hardware controls
      // You would add buttons, knobs, faders here
      
      // Example button
      HardwareButton playButton = hardwareSurface.createHardwareButton("PLAY");
      playButton.pressedAction().setActionMatcher(
         midiIn.createNoteOnActionMatcher(0, 0x5E) // Note 94 on channel 1
      );
      
      // Example knob
      AbsoluteHardwareKnob knob1 = hardwareSurface.createAbsoluteHardwareKnob("KNOB_1");
      knob1.setAdjustValueMatcher(
         midiIn.createAbsoluteCCValueMatcher(0, CC_KNOB_1)
      );
      
      // Example fader
      HardwareSlider fader1 = hardwareSurface.createHardwareSlider("FADER_1");
      fader1.setAdjustValueMatcher(
         midiIn.createAbsoluteCCValueMatcher(0, CC_FADER_1)
      );
   }
   
   private void setupBitwigAPI() {
      // Transport
      transport = host.createTransport();
      
      // Track bank for mixer control
      trackBank = host.createTrackBank(TRACK_COUNT, SEND_COUNT, SCENE_COUNT);
      
      // Cursor track for selected track
      cursorTrack = host.createCursorTrack("PC_MIDI_CURSOR_TRACK", "Cursor Track", 0, 0, true);
      
      // Follow track selection
      trackBank.followCursorTrack(cursorTrack);
      
      // Device control
      cursorDevice = cursorTrack.createCursorDevice("PC_MIDI_CURSOR_DEVICE", "Cursor Device", 0, CursorDeviceFollowMode.FOLLOW_SELECTION);
      primaryDevice = cursorTrack.createCursorDevice("PC_MIDI_PRIMARY_DEVICE", "Primary Device", 8, CursorDeviceFollowMode.FIRST_DEVICE);
      
      // Setup observers for visual feedback
      setupObservers();
   }
   
   private void setupObservers() {
      // Transport observers
      transport.isPlaying().addValueObserver(playing -> {
         // Update play button LED
         if (playing) {
            sendMidiCC(0, 0x5E, 127); // Example: Light up play button
         } else {
            sendMidiCC(0, 0x5E, 0);
         }
      });
      
      // Track observers
      for (int i = 0; i < TRACK_COUNT; i++) {
         Track track = trackBank.getItemAt(i);
         final int trackIndex = i;
         
         // Volume observer
         track.volume().value().addValueObserver(volume -> {
            // Update motorized fader or LED ring if available
            // int midiValue = (int)(volume * 127);
            // sendMidiCC(0, CC_FADER_1 + trackIndex, midiValue);
         });
         
         // Mute state observer
         track.mute().addValueObserver(muted -> {
            // Update mute button LED
            // sendMidiCC(0, CC_BUTTON_MUTE + trackIndex, muted ? 127 : 0);
         });
      }
   }
   
   private void setupMidiCallbacks() {
      // This is handled by the hardware surface bindings
      // Additional manual MIDI handling can be added here if needed
   }
   
   private void onMidi(int statusByte, int data1, int data2) {
      // Manual MIDI message handling
      // This is called for all MIDI messages not consumed by hardware controls
      
      int channel = statusByte & 0x0F;
      int status = statusByte & 0xF0;
      
      // Example: Handle CC messages
      if (status == 0xB0) { // CC message
         handleCC(channel, data1, data2);
      }
      // Example: Handle Note messages
      else if (status == 0x90 || status == 0x80) { // Note on/off
         handleNote(channel, data1, data2, status == 0x90);
      }
   }
   
   private void handleCC(int channel, int cc, int value) {
      // Handle CC messages
      // Example: Map CC to track volume
      if (cc >= CC_FADER_1 && cc < CC_FADER_1 + TRACK_COUNT) {
         int trackIndex = cc - CC_FADER_1;
         Track track = trackBank.getItemAt(trackIndex);
         track.volume().value().set(value / 127.0);
      }
      // Example: Map CC to device parameters
      else if (cc >= CC_KNOB_1 && cc < CC_KNOB_1 + 8) {
         int paramIndex = cc - CC_KNOB_1;
         Parameter param = primaryDevice.getParameter(paramIndex);
         param.value().set(value / 127.0);
      }
   }
   
   private void handleNote(int channel, int note, int velocity, boolean isNoteOn) {
      // Handle note messages for buttons
      if (!isNoteOn || velocity == 0) return;
      
      // Example: Transport controls
      switch (note) {
         case 0x5E: // Play
            transport.play();
            break;
         case 0x5D: // Stop
            transport.stop();
            break;
         case 0x5F: // Record
            transport.record();
            break;
         // Add more button mappings as needed
      }
   }
   
   private void onSysex(String sysex) {
      // Handle SysEx messages if your controller uses them
      host.println("Received SysEx: " + sysex);
   }
   
   private void initializeController() {
      // Send initialization messages to the controller
      // This might include setting LED states, display messages, etc.
      
      // Example: Reset all LEDs
      for (int i = 0; i < 128; i++) {
         sendMidiCC(0, i, 0);
      }
      
      // Example: Send welcome message if controller has a display
      // sendSysex("F0 00 20 29 02 18 14 00 50 43 20 4D 49 44 49 F7");
   }
   
   private void sendMidiCC(int channel, int cc, int value) {
      midiOut.sendMidi(0xB0 | channel, cc, value);
   }
   
   private void sendMidiNote(int channel, int note, int velocity) {
      midiOut.sendMidi(0x90 | channel, note, velocity);
   }
   
   private void sendSysex(String hex) {
      midiOut.sendSysex(hex);
   }
   
   @Override
   public void flush() {
      // Update hardware state (LEDs, displays, motorized faders)
      // This is called regularly by Bitwig
      
      // The hardware surface handles most updates automatically
      // Add any additional update logic here if needed
      hardwareSurface.updateHardware();
   }
   
   @Override
   public void exit() {
      // Cleanup when the extension is disabled
      host.println("PC MIDI Studio: Shutting down...");
      
      // Reset controller to default state
      initializeController();
      
      // Turn off all LEDs
      for (int i = 0; i < 128; i++) {
         sendMidiCC(0, i, 0);
      }
      
      host.println("PC MIDI Studio: Goodbye!");
   }
}