package audio.petitechose.controller;

import com.bitwig.extension.api.PlatformType;
import com.bitwig.extension.controller.AutoDetectionMidiPortNamesList;
import com.bitwig.extension.controller.ControllerExtension;
import com.bitwig.extension.controller.ControllerExtensionDefinition;
import com.bitwig.extension.controller.api.ControllerHost;

import java.util.UUID;

/**
 * Definition class for the PetiteChose MIDI Studio Controller Extension
 * This class provides metadata and configuration for the controller
 */
public class PCMidiStudioExtensionDefinition extends ControllerExtensionDefinition {
   
   // Extension metadata
   private static final UUID EXTENSION_ID = UUID.fromString("a1b2c3d4-e5f6-7890-abcd-ef1234567890");
   private static final String HARDWARE_VENDOR = "PetiteChose Audio";
   private static final String HARDWARE_MODEL = "PC MIDI Studio";
   private static final String EXTENSION_NAME = "PC MIDI Studio";
   private static final String AUTHOR = "PetiteChose Audio";
   private static final String VERSION = "1.0.0";
   private static final int REQUIRED_API_VERSION = 18; // Bitwig 5.0+
   
   // MIDI configuration
   private static final int NUM_MIDI_IN_PORTS = 1;
   private static final int NUM_MIDI_OUT_PORTS = 1;
   
   public PCMidiStudioExtensionDefinition() {
      // Constructor - no initialization needed for definition
   }
   
   @Override
   public UUID getId() {
      return EXTENSION_ID;
   }
   
   @Override
   public String getName() {
      return EXTENSION_NAME;
   }
   
   @Override
   public String getAuthor() {
      return AUTHOR;
   }
   
   @Override
   public String getVersion() {
      return VERSION;
   }
   
   @Override
   public String getHardwareVendor() {
      return HARDWARE_VENDOR;
   }
   
   @Override
   public String getHardwareModel() {
      return HARDWARE_MODEL;
   }
   
   @Override
   public int getRequiredAPIVersion() {
      return REQUIRED_API_VERSION;
   }
   
   @Override
   public int getNumMidiInPorts() {
      return NUM_MIDI_IN_PORTS;
   }
   
   @Override
   public int getNumMidiOutPorts() {
      return NUM_MIDI_OUT_PORTS;
   }
   
   @Override
   public void listAutoDetectionMidiPortNames(
         AutoDetectionMidiPortNamesList list,
         PlatformType platformType) {
      
      // Auto-detection configuration for different platforms
      // Adjust these patterns to match your actual MIDI device names
      
      switch (platformType) {
         case WINDOWS:
            list.add(
               new String[]{"PC MIDI Studio"},  // Input port names
               new String[]{"PC MIDI Studio"}   // Output port names
            );
            break;
            
         case MAC:
            list.add(
               new String[]{"PC MIDI Studio"},
               new String[]{"PC MIDI Studio"}
            );
            break;
            
         case LINUX:
            list.add(
               new String[]{"PC MIDI Studio MIDI 1"},
               new String[]{"PC MIDI Studio MIDI 1"}
            );
            break;
      }
   }
   
   @Override
   public ControllerExtension createInstance(ControllerHost host) {
      return new PCMidiStudioExtension(this, host);
   }
   
   @Override
   public String getHelpFilePath() {
      return "docs/PC_MIDI_Studio_Manual.pdf";
   }
   
   @Override
   public String getErrorReportingEMail() {
      return "support@petitechose.audio";
   }
   
   @Override
   public boolean shouldFailOnDeprecatedUse() {
      // Enable strict mode for development
      return true;
   }
}