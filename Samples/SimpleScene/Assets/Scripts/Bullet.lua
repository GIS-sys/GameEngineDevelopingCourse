local ecs = require "ecs"

local function destruction_timer(it)
  for timer, pos, ent in ecs.each(it) do
    if timer.enabled then
      timer.rest_time = timer.rest_time - it.delta_time
      if timer.rest_time <= 0 then
        pos.x = 10000
        timer.enabled = false
      end
    end
    pos.x = pos.x - it.delta_time * 0.2
    -- TODO delete this row ^
  end
end

local function collision(it)
  for col, pos, ent in ecs.each(it) do
    pos.z = pos.z + 0.002
    col.collided = false
    for col2, pos2, ent2 in ecs.each(it) do
      pos.z = pos.z - 0.002
      if pos.x ~= pos2.x or pos.y ~= pos2.y or pos.z ~= pos2.z then
        distance_squared = (pos.x - pos2.x)*(pos.x - pos2.x) + (pos.y - pos2.y)*(pos.y - pos2.y) + (pos.z - pos2.z)*(pos.z - pos2.z) + (col.size + col2.size)*(col.size + col2.size)
        if distance_squared < (col.size + col2.size)*(col.size + col2.size) then
          col.collided = true
          col2.collided = true
        end
      end
    end
  end
end

local function bullet_collision(it)
  for timer, col, pos, ent in ecs.each(it) do
    if col.collided then
      if timer.enabled == false then
        timer.rest_time = timer.start_time
        timer.enabled = true
      end
    end
  end
end

ecs.system(destruction_timer, "DestructionTimer", ecs.OnUpdate, "DestructionTimer, Position")
ecs.system(collision, "Collision", ecs.OnUpdate, "Collider, Position")
ecs.system(bullet_collision, "BulletCollision", ecs.OnUpdate, "DestructionTimer, Collider, Position")

