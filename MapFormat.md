# Map format specification #
```
<map version="1.0">
  <!-- Basic map configuration -->
  <properties>
    <!-- Map world physics -->
    <physics>
      <gravity>-9.81</gravity>
    </physics>
    
    <!-- Map script. This script is responsible for setting up the scene (by loading up
         objects), installing event handlers etc. -->
    <controller>/Scripts/map_controller</controller>
    
    <!-- Additional entity classes. These only need to be defined if the scene tag below
         is used (since controller script may load other scripts). -->
    <import>
      <script path="/Scripts/stupid_spaceship" class="Spaceship" />
      <script path="/Scripts/standard_entity_library" class="Switch,Door" />
    </import>
  </properties>
  
  <!-- Describes the scene as is prepared while loading this map. The map controller
       script can install hooks for scene pre-load and post-load to make any modifications
       necessary. Note that scene can be an empty element when map controller will
       generate the whole scene by itself. -->
  <scene>
    <!-- Here we instantiate new entities. Each entity has a model, texture and a class defined.
         When no class is defined the default iid.scene.Entity is used, otherwise classes must
         be predefined in the properties section of this map. -->
    <entity model="/Models/spaceship" texture="/Textures/spaceship" id="test">
      <pos>
        <x>0</x>
        <x>0</y>
        <z>0</z>
      </pos>
    </entity>
    
    <!-- Entities may also be fully scripted (position and all) -->
    <entity class="Spaceship" id="bar" />
    
    <!-- Composite models should specify subentity class -->
    <entity model="/Models/composite" class="MyCompositeEntity" id="composite">
      <!-- Subentity properties may be defined here. Match should contain a simplified regular
           expression that matches entity's identifier. -->
      <subentity match="wall*">
        <!-- Entity texture -->
        <texture>/Textures/stone</texture>
        
        <!-- Entity class -->
        <class>iid.scene.PhysicalEntity</class>
      </subentity>
      
      <!-- Defaults may be specified by using a * entry. It must be the last entry since they
           are matched in order! -->
      <subentity match="*">
        <texture>/Textures/default</texture>
        <class>iid.scene.PhysicalEntity</class>
      </subentity>
    </entity>
    
    <entity id="level">
      <subentity match="wall*">
        <texture>/Textures/stone</texture>
        <class>iid.scene.PhysicalEntity</class>
      </subentity>
      
      <subentity match="switch*">
        <texture>/Textures/switch</texture>
        <class>Switch</class>
      </subentity>
    </entity>
  </scene>
  
  <!-- Signal/slot connections -->
  <signals>
    <!-- Each connection specifies two entities by their scene hierarchy position composed of
         entity identifiers. It also specifies the signal name and the slot name (slots should
         be registred via decorators in entity behavior classes). -->
    <connect sender="/level/switch_x1" receiver="/level/door_x2">
      <signal>Switch.Changed</signal>
      <slot>Door.OpenOrClose</slot>
    </connect>
  </signals>
</map>
```