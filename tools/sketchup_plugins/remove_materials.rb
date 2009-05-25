# Copyright 2006, Todd Burch http://www.smustard.com 

=begin
Smustard.com(tm) Ruby Script End User License Agreement

This is a License Agreement is between you and Smustard.com.

If you download, acquire or purchase a Ruby Script or any freeware or any other product (collectively "Scripts") from Smustard.com, then you hereby accept and agree to all of the following terms and conditions:

Smustard.com, through its agreements with individual script authors, hereby grants you a permanent, worldwide, non-exclusive, non-transferable, non-sublicensable use license with respect to its rights in the Scripts.

If you are an individual, then you may copy the Scripts onto any computer you own at any location.

If you are an entity, then you may not copy the Scripts onto any other computer unless you purchase a separate license for each computer and you must have a separate license for the use of the Script on each computer.

You may not alter, publish, market, distribute, give, transfer, sell or sublicense the Scripts or any part of the Scripts.

This License Agreement is governed by the laws of the State of Texas and the United States of America.

You agree to submit to the jurisdiction of the Courts in Houston, Harris County, Texas, United States of America, to resolve any dispute, of any kind whatsoever, arising out of, involving or relating to this License Agreement.

THIS SOFTWARE IS PROVIDED "AS IS" AND WITHOUT ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, 
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.

This software has not been endorsed or sanctioned by Google.  Any comments, concerns or issues about this software 
or the affects of this software should be not directed to Google, but to Smustard.com.  
=end

#
# Name :          remove_materials.rb 1.0
# Description :   Remove all material from all selected objects.
# Author :        Todd Burch   http://www.smustard.com 
# Usage :         1. Select one of more entities. 
#                 2. Activate the script from the Plugins Menu 
# Date =          25.Oct.2006
# Type :          Plugin  
# History:        1.0 (25.Oct.2006) - first version
#
#-----------------------------------------------------------------------------
require 'sketchup.rb' 

class RemMats 

  def parse_ent(ent)  
    if ent.is_a? Sketchup::ComponentInstance then 
        ent.definition.entities.each {|ede| parse_ent(ede) } 
    elsif (ent.is_a? Sketchup::Group) then ent.entities.each {|ege| parse_ent(ege) }   
    end ; # if 
    ent.material      = nil if ent.respond_to?(:material) ;
    ent.back_material = nil if ent.respond_to?(:back_material) ; 
  end  # def parse_ent 

  def remove() ; 
    Sketchup.active_model.selection.each {|e| parse_ent(e) ; } 
  end ;  # def remove() 
end ;    # class RemMats

def remove_Materials_Smustard

  if Sketchup.active_model.selection.empty? then 
    UI.messagebox("Select Objects Before Running \'Remove All Materials\'.\n\n" +
                  "Use this script to remove all assigned materials\n" +  
                  "from all selected entities, groups and/or components." ) ; 
    return ; 
  end ; 
  Sketchup.active_model.start_operation("Delete All Materials") ; 
  x = RemMats.new ; 
  x.remove ; 
  Sketchup.active_model.commit_operation ; 

end ; 


if( not file_loaded?("remove_material.rb") )
    UI.menu("Plugins").add_item("Remove All Materials") { remove_Materials_Smustard }
end
#-----------------------------------------------------------------------------
file_loaded("remove_material.rb")

