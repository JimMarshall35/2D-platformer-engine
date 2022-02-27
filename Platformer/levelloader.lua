require "defs"
require "lualib"




function OnLoadLevel(path)
	luaLib_LoadLevel_PersistenceLib(path)
end

function OnSaveLevel(path)
	luaLib_SaveLevel_PersistenceLib(path)
end

function OnStartup(path)

end

function dump(o)
   if type(o) == 'table' then
      local s = '{ '
      for k,v in pairs(o) do
         if type(k) ~= 'number' then k = '"'..k..'"' end
         s = s .. '['..k..'] = ' .. dump(v) .. ','
      end
      return s .. '} '
   else
      return tostring(o)
   end
end


