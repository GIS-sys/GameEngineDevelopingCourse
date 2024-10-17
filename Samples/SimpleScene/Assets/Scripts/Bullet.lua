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
  -- https://github.com/flecs-hub/flecs-lua/blob/master/test/system.lua
  local col, pos, vels = ecs.columns(it)
  for i = 1, it.count do
    col[i].collided = false
  end
  for i = 1, it.count do
    --if it.count == 1 then -- TODO debug, this will show that objects are not in the same set
    --  pos[i].z = pos[i].z + 0.5 * it.delta_time
    --end
    if col[i].collided == false then
      for j = 1, it.count do
        if i ~= j then
          distance_squared = (pos[i].x - pos[j].x)*(pos[i].x - pos[j].x) + (pos[i].y - pos[j].y)*(pos[i].y - pos[j].y) + (pos[i].z - pos[j].z)*(pos[i].z - pos[j].z)
          total_size_squared = (col[i].size + col[j].size)*(col[i].size + col[j].size)
          if distance_squared < total_size_squared then
            col[i].collided = true
            col[j].collided = true
            vels[i].x = -vels[i].x
            vels[i].y = -vels[i].y
            vels[i].z = -vels[i].z
            vels[j].x = -vels[j].x
            vels[j].y = -vels[j].y
            vels[j].z = -vels[j].z
          end
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

local function non_bullet_collision(it)
  for col, vel, ent in ecs.each(it) do
    if col.collided then
      --vel.x = vel.x + 0.1;
      vel.y = vel.y + 1.0;
      --vel.z = vel.z + 0.1;
    end
  end
end

ecs.system(destruction_timer, "DestructionTimer", ecs.OnUpdate, "DestructionTimer, Position")
ecs.system(collision, "Collision", ecs.OnUpdate, "Collider, Position, Velocity")
ecs.system(bullet_collision, "BulletCollision", ecs.OnUpdate, "DestructionTimer, Collider, Position")
ecs.system(non_bullet_collision, "NonBulletCollision", ecs.OnUpdate, "Collider, Velocity")

