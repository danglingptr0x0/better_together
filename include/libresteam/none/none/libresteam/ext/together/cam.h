#ifndef BETTER_TOGETHER_CAM_H
#define BETTER_TOGETHER_CAM_H

#define BETTER_TOGETHER_CAM_SCRIPT \
        "local host,guest,epoch,now=%u32,%u32,%u32,%u64\n" \
        "local st=rawget(_G,\"__bt_ext_cam\")\n" \
        "if type(st)~=\"table\" then st={}; rawset(_G,\"__bt_ext_cam\",st) end\n" \
        "st.wave_now=now/1000\n" \
        "local function stop()\n" \
        "    if st.look_stop then st.look_stop(); st.look_stop=nil end\n" \
        "    if st.func and st.env then setfenv(st.func,st.env) end\n" \
        "    st.func=nil; st.proxy=nil; st.env=nil; st.dist=nil; st.probe=nil; st.probe_result=nil\n" \
        "    st.limit_state=0; st.limit_err=nil; st.limit_hits=0\n" \
        "    st.wave_state=0; st.wave_outside_since=nil; st.wave_pending=false; st.wave_last=nil\n" \
        "end\n" \
        "local function report(state,val)\n" \
        "    if state==1 and st.limit_err then state=2; val=st.limit_err end\n" \
        "    if state==1 and st.look_err then state=2; val=st.look_err end\n" \
        "    if state==1 and st.wave_err then state=2; val=st.wave_err end\n" \
        "    return state,val,st.attach_state or 0,st.attach_err or \"\"\n" \
        "end\n" \
        "if st.host~=host or st.guest~=guest or st.epoch~=epoch then\n" \
        "    stop(); st.host=host; st.guest=guest; st.epoch=epoch; st.err=nil\n" \
        "    st.attach_state=0; st.attach_err=nil; st.attach_steps=0; st.attach_cunt=nil\n" \
        "    st.wave_next=st.wave_next or 0; st.wave_hits=st.wave_hits or 0; st.wave_err=nil\n" \
        "end\n" \
        "if st.err then return report(2,st.err) end\n" \
        "local cameras=camera.GetActiveCameras()\n" \
        "local f\n" \
        "for i=0,cameras:GetCount()-1 do\n" \
        "    local c=cameras:GetCamera(i)\n" \
        "    if camera.IsMostInfluentialCamera(c) then\n" \
        "        local b=GetBehavior(c,\"CameraComponentFollowYarny\")\n" \
        "        if type(b)==\"table\" then f=b.CameraPreRenderUpdate0 end\n" \
        "        break\n" \
        "    end\n" \
        "end\n" \
        "if type(f)~=\"function\" then stop(); return report(0,0) end\n" \
        "if st.func==f and st.proxy and st.env then\n" \
        "    if getfenv(f)~=st.proxy then setfenv(f,st.proxy) end\n" \
        "    return report(1,st.dist or -1)\n" \
        "end\n" \
        "stop()\n" \
        "local env=getfenv(f)\n" \
        "local get=env.transform.GetTranslation\n" \
        "local player\n" \
        "for i=1,32 do\n" \
        "    local n,v=debug.getupvalue(f,i)\n" \
        "    if not n then break end\n" \
        "    if n==\"m_player\" then player=v; break end\n" \
        "end\n" \
        "if not player or not player:Alive() or player:GetUniqueID()~=host then return report(0,0) end\n" \
        "local proxy\n" \
        "local function guest_get()\n" \
        "    local b=env.FindGameObject(\"Boxie\")\n" \
        "    if not b or not b:Alive() or b:GetUniqueID()~=guest then return nil end\n" \
        "    return b\n" \
        "end\n" \
        "local function attach_step()\n" \
        "    if st.attach_state>=2 then return end\n" \
        "    st.attach_steps=st.attach_steps+1\n" \
        "    assert(st.attach_steps<=600,\"Attachment readiness timeout\")\n" \
        "    local a=env.FindGameObject(\"Yarny\")\n" \
        "    local b=guest_get()\n" \
        "    if not a or not a:Alive() or a:GetUniqueID()~=host or not b then return end\n" \
        "    if not env.box2d.HasPhysics(a) or not env.box2d.HasPhysics(b) then return end\n" \
        "    local yarn=env.yarn\n" \
        "    if st.attach_state==0 then\n" \
        "        local p=get(b)\n" \
        "        local o=yarn.AnyAttachmentClose(env.Vector3(p.x,p.y,p.z),0.5,false)\n" \
        "        if o and o:Alive() and o:Equals(b) then st.attach_state=2; return end\n" \
        "        local attachments=yarn.GetNumAttachments(1)\n" \
        "        if attachments==0 then return end\n" \
        "        assert(attachments==1,\"Existing yarn attachments\")\n" \
        "        st.attach_cunt=attachments\n" \
        "        assert(yarn.StartAttachYarn(b,env.Vector3(0,0,0.075),false,0,false),\"Attachment start rejected\")\n" \
        "        st.attach_state=1\n" \
        "        return\n" \
        "    end\n" \
        "    assert(yarn.GetNumAttachments(1)==st.attach_cunt,\"Attachment changed while waiting\")\n" \
        "    if not yarn.IsReadyToPrepareAttachYarn(a) then return end\n" \
        "    yarn.PrepareAttachYarn(1)\n" \
        "    yarn.AttachYarn()\n" \
        "    assert(yarn.GetNumAttachments(1)==st.attach_cunt+1,\"Attachment commit failed\")\n" \
        "    st.attach_state=2\n" \
        "end\n" \
        "local LIMIT_BOUND=0.9\n" \
        "local LIMIT_STEP=0.25\n" \
        "local LIMIT_DEPTH_FRACTION=0.25\n" \
        "local LIMIT_EPS=0.00001\n" \
        "local LIMIT_CONTACT=0.0002\n" \
        "local LIMIT_DEPTH=0.001\n" \
        "local LIMIT_CORRECTION=0.5\n" \
        "local function finite_is(val)\n" \
        "    return type(val)==\"number\" and val==val and math.abs(val)<math.huge\n" \
        "end\n" \
        "local function xyz_get(vec)\n" \
        "    local x,y,z=vec.x,vec.y,vec.z\n" \
        "    assert(finite_is(x) and finite_is(y) and finite_is(z),\"Invalid frustum vector\")\n" \
        "    return x,y,z\n" \
        "end\n" \
        "local LOOK_RADIUS=0.75\n" \
        "local LOOK_INPUT=0.1\n" \
        "local LOOK_PEEK=0.01\n" \
        "local LOOK_SPEED=0.05\n" \
        "local LOOK_UPVALUE_CAP=100\n" \
        "local LOOK_POI_CAP=256\n" \
        "local observers={}\n" \
        "st.look_state=0; st.look_hits=0; st.look_err=nil\n" \
        "if st.look_radius==nil then st.look_radius=LOOK_RADIUS end\n" \
        "local function upvalue_slot(func,name)\n" \
        "    for idx=1,LOOK_UPVALUE_CAP do\n" \
        "        local key=debug.getupvalue(func,idx)\n" \
        "        if not key then break end\n" \
        "        if key==name then return idx end\n" \
        "    end\n" \
        "    error(\"Missing gaze upvalue: \"..name)\n" \
        "end\n" \
        "local function slot_get(func,idx)\n" \
        "    local _,val=debug.getupvalue(func,idx)\n" \
        "    return val\n" \
        "end\n" \
        "local function observer_bind(script,ident,tgt_name,tgt_id)\n" \
        "    local orig=assert(script.HandlePOI,\"Missing HandlePOI\")\n" \
        "    local idle=assert(script.HandleIdleAnimations,\"Missing HandleIdleAnimations\")\n" \
        "    local original_env=getfenv(orig)\n" \
        "    assert(rawget(original_env,\"HandlePOI\")==orig,\"Unexpected HandlePOI environment\")\n" \
        "    local slots={}\n" \
        "    for _,key in ipairs({\"m_state\",\"e_state_normal\",\"m_current_walking_animation\",\"mc_undefined_h\",\"m_running\",\"m_walking_input\",\"m_pushing\",\"m_climbing_input\",\"m_aiming_input\",\"m_num_pois\",\"m_pois\",\"m_look_at_position\"}) do slots[key]=upvalue_slot(orig,key) end\n" \
        "    local idle_slot=upvalue_slot(idle,\"m_current_idle_animation\")\n" \
        "    local event_slot=upvalue_slot(idle,\"m_event_animation\")\n" \
        "    local function val_get(key) return slot_get(orig,slots[key]) end\n" \
        "    local handle=original_env.StoredGameObjectHandle()\n" \
        "    local entry={m_object=handle,m_refs=1,m_prio_cd=0}\n" \
        "    local head_hash=original_env.ComputeHash(\"Head\")\n" \
        "    local record={script=script,orig=orig,active=false,failed=false}\n" \
        "    local inserted\n" \
        "    local function poi_drop()\n" \
        "        if not inserted then return end\n" \
        "        local list=val_get(\"m_pois\")\n" \
        "        local cunt=val_get(\"m_num_pois\")\n" \
        "        assert(type(cunt)==\"number\" and cunt>=0 and cunt<=LOOK_POI_CAP and cunt==math.floor(cunt),\"Invalid gaze POI count\")\n" \
        "        for idx=1,cunt do\n" \
        "            if list[idx]==entry then\n" \
        "                list[idx]=list[cunt]; list[cunt]=nil\n" \
        "                debug.setupvalue(orig,slots.m_num_pois,cunt-1)\n" \
        "                break\n" \
        "            end\n" \
        "        end\n" \
        "        inserted=false\n" \
        "    end\n" \
        "    local function eligible_is(actor)\n" \
        "        if not actor or not actor:Alive() or actor:GetUniqueID()~=ident then return false end\n" \
        "        if script.m_on_ground~=true or script.m_ragdoll_enabled or script.m_out_of_yarn or script.m_out_of_yarn_action or (script.m_shooting_yarn and script.m_shooting_yarn~=0) or script.m_unstable then return false end\n" \
        "        if val_get(\"m_state\")~=val_get(\"e_state_normal\") or val_get(\"m_running\") or val_get(\"m_pushing\") then return false end\n" \
        "        local undefined=val_get(\"mc_undefined_h\")\n" \
        "        if val_get(\"m_current_walking_animation\")~=undefined or slot_get(idle,event_slot)~=undefined then return false end\n" \
        "        if math.abs(val_get(\"m_walking_input\"))>=LOOK_INPUT or math.abs(val_get(\"m_climbing_input\"))>=LOOK_INPUT then return false end\n" \
        "        local aim=val_get(\"m_aiming_input\")\n" \
        "        if math.abs(aim.x)>=LOOK_INPUT or math.abs(aim.z)>=LOOK_INPUT then return false end\n" \
        "        if math.abs(script.m_peek_offset_x or 0)>=LOOK_PEEK or math.abs(script.m_peek_offset_y or 0)>=LOOK_PEEK or script.m_climbdown_oneway_allowed then return false end\n" \
        "        local anim=slot_get(idle,idle_slot)\n" \
        "        if anim==nil or anim==undefined then return false end\n" \
        "        if anim~=script.m_anim_idle_0_r_h and anim~=script.m_anim_idle_0_l_h and anim~=script.m_anim_idle_1_r_h and anim~=script.m_anim_idle_1_l_h then return false end\n" \
        "        if not original_env.box2d.HasPhysics(actor) then return false end\n" \
        "        local vx,vy,vz=xyz_get(original_env.box2d.GetVelocity(actor))\n" \
        "        return vx*vx+vy*vy+vz*vz<=LOOK_SPEED*LOOK_SPEED\n" \
        "    end\n" \
        "    local function target_get()\n" \
        "        local tgt=original_env.FindGameObject(tgt_name)\n" \
        "        if tgt and tgt:Alive() and tgt:GetUniqueID()==tgt_id then return tgt end\n" \
        "    end\n" \
        "    local function prepare(actor)\n" \
        "        record.active=false\n" \
        "        if not eligible_is(actor) or val_get(\"m_num_pois\")~=0 then return false end\n" \
        "        local tgt=target_get()\n" \
        "        if not tgt then return false end\n" \
        "        local radius=st.look_radius\n" \
        "        assert(finite_is(radius) and radius>0,\"Invalid gaze radius\")\n" \
        "        local x,y,z=xyz_get(original_env.transform.GetTranslation(actor))\n" \
        "        local tx,ty,tz=xyz_get(original_env.transform.GetTranslation(tgt))\n" \
        "        local dx,dy,dz=tx-x,ty-y,tz-z\n" \
        "        if dx*dx+dy*dy+dz*dz>radius*radius then return false end\n" \
        "        handle:Set(tgt)\n" \
        "        local list=val_get(\"m_pois\")\n" \
        "        assert(type(list)==\"table\",\"Invalid gaze POI table\")\n" \
        "        list[1]=entry; inserted=true\n" \
        "        debug.setupvalue(orig,slots.m_num_pois,1)\n" \
        "        return true\n" \
        "    end\n" \
        "    local function finish(actor,selected)\n" \
        "        poi_drop()\n" \
        "        if not selected or not eligible_is(actor) then return end\n" \
        "        local tgt=target_get()\n" \
        "        if not tgt then return end\n" \
        "        local x,y,z=xyz_get(original_env.renderable.GetBoneWorldTranslate(tgt,head_hash))\n" \
        "        val_get(\"m_look_at_position\"):SetVec(original_env.Vector3(x,y,z))\n" \
        "        record.active=true\n" \
        "        st.look_hits=st.look_hits+1\n" \
        "    end\n" \
        "    local wrapper\n" \
        "    local function restore()\n" \
        "        record.active=false; record.failed=true\n" \
        "        if original_env.HandlePOI==wrapper then original_env.HandlePOI=orig end\n" \
        "        poi_drop()\n" \
        "    end\n" \
        "    local function fail(err)\n" \
        "        st.look_err=tostring(err); st.look_state=2\n" \
        "        local ok,cleanup_err=pcall(restore)\n" \
        "        if not ok then st.look_err=st.look_err..\"; \"..tostring(cleanup_err) end\n" \
        "    end\n" \
        "    wrapper=function(actor,...)\n" \
        "        if record.failed or st.epoch~=epoch or st.func~=f or st.proxy~=proxy then return orig(actor,...) end\n" \
        "        local prepared,selected=pcall(prepare,actor)\n" \
        "        if not prepared then fail(selected); return orig(actor,...) end\n" \
        "        local ok,err=pcall(orig,actor,...)\n" \
        "        local finished,finish_err=pcall(finish,actor,ok and selected)\n" \
        "        if not finished then fail(finish_err) end\n" \
        "        if not ok then fail(err); error(err,0) end\n" \
        "    end\n" \
        "    record.wrapper=wrapper; record.stop=restore\n" \
        "    record.greeting_ready=function(actor) return not record.failed and eligible_is(actor) and val_get(\"m_num_pois\")==0 end\n" \
        "    original_env.HandlePOI=wrapper\n" \
        "    return record\n" \
        "end\n" \
        "local function look_stop()\n" \
        "    for idx=1,2 do\n" \
        "        local record=observers[idx]\n" \
        "        if record then\n" \
        "            local ok,err=pcall(record.stop)\n" \
        "            if not ok then st.look_err=tostring(err) end\n" \
        "            observers[idx]=nil\n" \
        "        end\n" \
        "    end\n" \
        "    st.look_state=0\n" \
        "end\n" \
        "st.look_stop=look_stop\n" \
        "local function observer_update(idx,actor,ident,tgt_name,tgt_id)\n" \
        "    local script=actor and env.GetBehavior(actor,\"YarnyScript\")\n" \
        "    local record=observers[idx]\n" \
        "    if record and record.script==script and (script.HandlePOI==record.wrapper or script.HandlePOI==record.orig) then return end\n" \
        "    st.wave_state=0; st.wave_outside_since=nil; st.wave_pending=false\n" \
        "    if record then record.stop(); observers[idx]=nil end\n" \
        "    if type(script)==\"table\" then observers[idx]=observer_bind(script,ident,tgt_name,tgt_id) end\n" \
        "end\n" \
        "local function look_tick()\n" \
        "    if st.look_err then return end\n" \
        "    local actor=env.FindGameObject(\"Yarny\")\n" \
        "    local tgt=guest_get()\n" \
        "    if not actor or not actor:Alive() or actor:GetUniqueID()~=host or not tgt then look_stop(); return end\n" \
        "    observer_update(1,actor,host,\"Boxie\",guest)\n" \
        "    observer_update(2,tgt,guest,\"Yarny\",host)\n" \
        "    st.look_state=observers[1] and observers[2] and 1 or 0\n" \
        "end\n" \
        "local WAVE_SPEED=0.5\n" \
        "local WAVE_FADE_IN=0.05\n" \
        "local WAVE_FADE_OUT=0.2\n" \
        "local WAVE_OUTSIDE_TIME=10\n" \
        "local WAVE_COOLDOWN=600\n" \
        "local WAVE_CLOCK_GAP=2.5\n" \
        "if st.wave_radius==nil then st.wave_radius=LOOK_RADIUS end\n" \
        "if st.wave_outside_time==nil then st.wave_outside_time=WAVE_OUTSIDE_TIME end\n" \
        "if st.wave_cooldown==nil then st.wave_cooldown=WAVE_COOLDOWN end\n" \
        "local function wave_reset()\n" \
        "    st.wave_state=0; st.wave_outside_since=nil; st.wave_pending=false\n" \
        "end\n" \
        "local function wave_clip_get(script)\n" \
        "    local shooting=assert(script.HandleShootingNew,\"Missing greeting facing callback\")\n" \
        "    local forward=slot_get(shooting,upvalue_slot(shooting,\"m_wanted_forward\"))\n" \
        "    assert(forward and finite_is(forward.x),\"Invalid greeting facing\")\n" \
        "    local clip\n" \
        "    if forward.x>0 then clip=script.m_anim_mosquito_r_h else clip=script.m_anim_mosquito_l_h end\n" \
        "    assert(finite_is(clip),\"Missing greeting clip\")\n" \
        "    return clip\n" \
        "end\n" \
        "local function wave_play(actor,clip)\n" \
        "    local copy=env.animation.PlayCopyOnce(actor,clip,WAVE_FADE_IN,WAVE_FADE_OUT,true,true,0,0)\n" \
        "    assert(copy~=nil,\"Greeting playback returned no copy\")\n" \
        "    env.animation.SetAnimationSpeed(actor,copy,WAVE_SPEED)\n" \
        "end\n" \
        "local function wave_tick()\n" \
        "    if st.wave_err then return end\n" \
        "    local clock=st.wave_now\n" \
        "    assert(finite_is(clock),\"Invalid greeting clock\")\n" \
        "    if st.wave_last and (clock<st.wave_last or clock-st.wave_last>WAVE_CLOCK_GAP) then wave_reset() end\n" \
        "    st.wave_last=clock\n" \
        "    local primary,secondary=observers[1],observers[2]\n" \
        "    local actor=env.FindGameObject(\"Yarny\")\n" \
        "    local other=guest_get()\n" \
        "    if not actor or not actor:Alive() or actor:GetUniqueID()~=host or not other or not primary or not secondary or primary.failed or secondary.failed then wave_reset(); return end\n" \
        "    local radius,outside,cooldown=st.wave_radius,st.wave_outside_time,st.wave_cooldown\n" \
        "    assert(finite_is(radius) and radius>0 and finite_is(outside) and outside>0 and finite_is(cooldown) and cooldown>0,\"Invalid greeting settings\")\n" \
        "    local x,y,z=xyz_get(get(actor))\n" \
        "    local tx,ty,tz=xyz_get(get(other))\n" \
        "    local dx,dy,dz=tx-x,ty-y,tz-z\n" \
        "    if dx*dx+dy*dy+dz*dz>radius*radius then\n" \
        "        if st.wave_state~=2 then st.wave_outside_since=clock end\n" \
        "        st.wave_state=2; st.wave_pending=false\n" \
        "        return\n" \
        "    end\n" \
        "    if st.wave_state==2 then\n" \
        "        st.wave_pending=clock-st.wave_outside_since>=outside and clock>=st.wave_next\n" \
        "        st.wave_outside_since=nil\n" \
        "    end\n" \
        "    st.wave_state=st.wave_pending and 3 or 1\n" \
        "    if not st.wave_pending or not primary.greeting_ready(actor) or not secondary.greeting_ready(other) then return end\n" \
        "    local primary_clip=wave_clip_get(primary.script)\n" \
        "    local secondary_clip=wave_clip_get(secondary.script)\n" \
        "    assert(type(env.animation.PlayCopyOnce)==\"function\" and type(env.animation.SetAnimationSpeed)==\"function\",\"Missing greeting playback API\")\n" \
        "    st.wave_pending=false; st.wave_state=1; st.wave_next=clock+cooldown\n" \
        "    wave_play(actor,primary_clip)\n" \
        "    wave_play(other,secondary_clip)\n" \
        "    st.wave_hits=st.wave_hits+1\n" \
        "end\n" \
        "local function feasible_is(planes,x,z)\n" \
        "    for idx=1,#planes do\n" \
        "        local plane=planes[idx]\n" \
        "        if plane[1]*x+plane[2]*z>plane[3]+LIMIT_EPS then return false end\n" \
        "    end\n" \
        "    return true\n" \
        "end\n" \
        "local function nearest_get(planes,x,z)\n" \
        "    if feasible_is(planes,x,z) then return x,z end\n" \
        "    local best_x,best_z,best_dist\n" \
        "    local function consider(pos_x,pos_z)\n" \
        "        if not finite_is(pos_x) or not finite_is(pos_z) or not feasible_is(planes,pos_x,pos_z) then return end\n" \
        "        local delta_x,delta_z=pos_x-x,pos_z-z\n" \
        "        local dist=delta_x*delta_x+delta_z*delta_z\n" \
        "        if not best_dist or dist<best_dist then best_x,best_z,best_dist=pos_x,pos_z,dist end\n" \
        "    end\n" \
        "    for idx=1,#planes do\n" \
        "        local plane=planes[idx]\n" \
        "        local norm=plane[1]*plane[1]+plane[2]*plane[2]\n" \
        "        if norm>LIMIT_EPS*LIMIT_EPS then\n" \
        "            local delta=(plane[1]*x+plane[2]*z-plane[3])/norm\n" \
        "            consider(x-delta*plane[1],z-delta*plane[2])\n" \
        "            for other_idx=idx+1,#planes do\n" \
        "                local other=planes[other_idx]\n" \
        "                local determinant=plane[1]*other[2]-plane[2]*other[1]\n" \
        "                if math.abs(determinant)>LIMIT_EPS then\n" \
        "                    consider((plane[3]*other[2]-plane[2]*other[3])/determinant,(plane[1]*other[3]-plane[3]*other[1])/determinant)\n" \
        "                end\n" \
        "            end\n" \
        "        end\n" \
        "    end\n" \
        "    return best_x,best_z\n" \
        "end\n" \
        "local bodies={{planes={},cone={}},{planes={},cone={}}}\n" \
        "for idx=1,2 do\n" \
        "    for plane_idx=1,6 do bodies[idx].planes[plane_idx]={}; bodies[idx].cone[plane_idx]={} end\n" \
        "end\n" \
        "local function plane_set(plane,x,z,bound)\n" \
        "    local norm=math.sqrt(x*x+z*z)\n" \
        "    assert(finite_is(norm) and finite_is(bound),\"Invalid frustum plane\")\n" \
        "    if norm>LIMIT_EPS then\n" \
        "        plane[1],plane[2],plane[3]=x/norm,z/norm,bound/norm\n" \
        "    else\n" \
        "        plane[1],plane[2],plane[3]=0,0,bound>=0 and 1 or -1\n" \
        "    end\n" \
        "end\n" \
        "local function body_prepare(body,object,eye_x,eye_y,eye_z,dir_x,dir_y,dir_z)\n" \
        "    body.object=object\n" \
        "    local phys=env.box2d\n" \
        "    local pos_x,pos_y,pos_z=xyz_get(phys.GetWorldPosition(object,env.Vector3(0,0,0)))\n" \
        "    local vel_x,vel_y,vel_z=xyz_get(phys.GetVelocity(object))\n" \
        "    body.x,body.y,body.z=pos_x,pos_y,pos_z\n" \
        "    body.vx,body.vy,body.vz=vel_x,vel_y,vel_z\n" \
        "    local depth=(pos_x-eye_x)*dir_x+(pos_y-eye_y)*dir_y+(pos_z-eye_z)*dir_z\n" \
        "    if depth<=LIMIT_DEPTH then return false end\n" \
        "    local step=math.min(LIMIT_STEP,depth*LIMIT_DEPTH_FRACTION)\n" \
        "    local screen_x,screen_y,screen_z=xyz_get(env.camera.WorldToScreen(env.Vector3(pos_x,pos_y,pos_z)))\n" \
        "    if screen_z<=0 or screen_z>=1 then return false end\n" \
        "    local sample_x,sample_y,sample_z=xyz_get(env.camera.WorldToScreen(env.Vector3(pos_x+step,pos_y,pos_z)))\n" \
        "    local depth_x=depth+step*dir_x\n" \
        "    local grad_xx=(sample_x*depth_x-screen_x*depth)/step\n" \
        "    local grad_yx=(sample_y*depth_x-screen_y*depth)/step\n" \
        "    local grad_zx=(sample_z*depth_x-screen_z*depth)/step\n" \
        "    sample_x,sample_y,sample_z=xyz_get(env.camera.WorldToScreen(env.Vector3(pos_x,pos_y,pos_z+step)))\n" \
        "    local depth_z=depth+step*dir_z\n" \
        "    local grad_xz=(sample_x*depth_z-screen_x*depth)/step\n" \
        "    local grad_yz=(sample_y*depth_z-screen_y*depth)/step\n" \
        "    local grad_zz=(sample_z*depth_z-screen_z*depth)/step\n" \
        "    local planes=body.planes\n" \
        "    plane_set(planes[1],grad_xx-LIMIT_BOUND*dir_x,grad_xz-LIMIT_BOUND*dir_z,(LIMIT_BOUND-screen_x)*depth)\n" \
        "    plane_set(planes[2],-grad_xx-LIMIT_BOUND*dir_x,-grad_xz-LIMIT_BOUND*dir_z,(LIMIT_BOUND+screen_x)*depth)\n" \
        "    plane_set(planes[3],grad_yx-LIMIT_BOUND*dir_x,grad_yz-LIMIT_BOUND*dir_z,(LIMIT_BOUND-screen_y)*depth)\n" \
        "    plane_set(planes[4],-grad_yx-LIMIT_BOUND*dir_x,-grad_yz-LIMIT_BOUND*dir_z,(LIMIT_BOUND+screen_y)*depth)\n" \
        "    plane_set(planes[5],-grad_zx,-grad_zz,screen_z*depth)\n" \
        "    plane_set(planes[6],grad_zx-dir_x,grad_zz-dir_z,(1-screen_z)*depth)\n" \
        "    body.inside=feasible_is(planes,0,0)\n" \
        "    local delta_x,delta_z=nearest_get(planes,0,0)\n" \
        "    if not delta_x or delta_x*delta_x+delta_z*delta_z>LIMIT_CORRECTION*LIMIT_CORRECTION then return false end\n" \
        "    body.dx,body.dz=delta_x,delta_z\n" \
        "    local cone=body.cone\n" \
        "    for idx=1,#planes do\n" \
        "        local plane=planes[idx]\n" \
        "        if plane[3]-plane[1]*delta_x-plane[2]*delta_z<=LIMIT_CONTACT then\n" \
        "            cone[idx][1],cone[idx][2],cone[idx][3]=plane[1],plane[2],0\n" \
        "        else\n" \
        "            cone[idx][1],cone[idx][2],cone[idx][3]=0,0,1\n" \
        "        end\n" \
        "    end\n" \
        "    body.out_vx,body.out_vz=nearest_get(cone,vel_x,vel_z)\n" \
        "    if not body.out_vx then return false end\n" \
        "    if delta_x~=0 or delta_z~=0 then\n" \
        "        local clip_x,clip_y,clip_z=xyz_get(env.camera.WorldToScreen(env.Vector3(pos_x+delta_x,pos_y,pos_z+delta_z)))\n" \
        "        if math.abs(clip_x)>LIMIT_BOUND+LIMIT_CONTACT or math.abs(clip_y)>LIMIT_BOUND+LIMIT_CONTACT or clip_z<=0 or clip_z>=1 then return false end\n" \
        "    end\n" \
        "    return true\n" \
        "end\n" \
        "local function body_apply(body)\n" \
        "    local moved=body.dx~=0 or body.dz~=0\n" \
        "    local slowed=math.abs(body.vx-body.out_vx)>LIMIT_EPS or math.abs(body.vz-body.out_vz)>LIMIT_EPS\n" \
        "    if moved then env.box2d.SetTranslation(body.object,env.Vector3(body.x+body.dx,body.y,body.z+body.dz)) end\n" \
        "    if slowed then env.box2d.SetVelocity(body.object,env.Vector3(body.out_vx,body.vy,body.out_vz)) end\n" \
        "    if moved or slowed then st.limit_hits=st.limit_hits+1 end\n" \
        "end\n" \
        "local function limit_tick()\n" \
        "    if st.limit_err then return end\n" \
        "    local cameras=env.camera.GetActiveCameras()\n" \
        "    local cam_obj\n" \
        "    for idx=0,cameras:GetCount()-1 do\n" \
        "        local candidate=cameras:GetCamera(idx)\n" \
        "        if candidate:Alive() and env.camera.IsMostInfluentialCamera(candidate) then cam_obj=candidate; break end\n" \
        "    end\n" \
        "    if not cam_obj then st.limit_state=0; return end\n" \
        "    local guest_object=guest_get()\n" \
        "    if not player:Alive() or player:GetUniqueID()~=host or not guest_object or not cam_obj:Alive() or not env.camera.IsMostInfluentialCamera(cam_obj) then st.limit_state=0; return end\n" \
        "    if not env.box2d.HasPhysics(player) or not env.box2d.HasPhysics(guest_object) then st.limit_state=0; return end\n" \
        "    local eye_x,eye_y,eye_z=xyz_get(env.camera.GetPosition())\n" \
        "    local up_x,up_y,up_z=xyz_get(env.camera.GetUp())\n" \
        "    local norm=math.sqrt(up_x*up_x+up_y*up_y+up_z*up_z)\n" \
        "    assert(finite_is(norm) and norm>LIMIT_EPS,\"Invalid camera viewing axis\")\n" \
        "    local dir_x,dir_y,dir_z=-up_x/norm,-up_y/norm,-up_z/norm\n" \
        "    local host_ready=body_prepare(bodies[1],player,eye_x,eye_y,eye_z,dir_x,dir_y,dir_z)\n" \
        "    local guest_ready=body_prepare(bodies[2],guest_object,eye_x,eye_y,eye_z,dir_x,dir_y,dir_z)\n" \
        "    if not host_ready or not guest_ready then st.limit_state=0; return end\n" \
        "    if st.limit_state~=1 and (not bodies[1].inside or not bodies[2].inside) then return end\n" \
        "    st.limit_state=1\n" \
        "    body_apply(bodies[1]); body_apply(bodies[2])\n" \
        "end\n" \
        "local function midpoint(p)\n" \
        "    local x,y,z=p.x,p.y,p.z\n" \
        "    local b=guest_get()\n" \
        "    if not b then return p end\n" \
        "    local q=get(b)\n" \
        "    local bx,bz=q.x,q.z\n" \
        "    local dx,dz=bx-x,bz-z\n" \
        "    local dist=math.sqrt(dx*dx+dz*dz)\n" \
        "    assert(dist==dist and dist<math.huge,\"Invalid camera distance\")\n" \
        "    st.dist=dist\n" \
        "    return env.Vector3((x+bx)/2,y,(z+bz)/2)\n" \
        "end\n" \
        "local function update(o)\n" \
        "    local primary=o and o:Alive() and o:GetUniqueID()==host\n" \
        "    if primary and st.probe then\n" \
        "        local probe=st.probe; st.probe=nil\n" \
        "        local res={pcall(probe,env)}\n" \
        "        if st.func==f and st.proxy==proxy and st.epoch==epoch then st.probe_result=res end\n" \
        "    end\n" \
        "    if st.func~=f or st.proxy~=proxy or st.epoch~=epoch then return get(o) end\n" \
        "    if primary and st.attach_state<2 then\n" \
        "        local ok,err=pcall(attach_step)\n" \
        "        if not ok then st.attach_state=3; st.attach_err=tostring(err) end\n" \
        "    end\n" \
        "    if primary and not st.err then\n" \
        "        local ok,err=pcall(look_tick)\n" \
        "        if not ok then look_stop(); st.look_state=2; st.look_err=tostring(err) end\n" \
        "    end\n" \
        "    if primary and not st.err then\n" \
        "        local ok,err=pcall(limit_tick)\n" \
        "        if not ok then st.limit_state=2; st.limit_err=tostring(err) end\n" \
        "    end\n" \
        "    if primary and not st.err then\n" \
        "        local ok,err=pcall(wave_tick)\n" \
        "        if not ok then wave_reset(); st.wave_state=4; st.wave_err=tostring(err) end\n" \
        "    end\n" \
        "    local p=get(o)\n" \
        "    if primary and not st.err then\n" \
        "        local ok,val=pcall(midpoint,p)\n" \
        "        if ok then p=val else st.err=tostring(val) end\n" \
        "    end\n" \
        "    return p\n" \
        "end\n" \
        "local busy=false\n" \
        "local function read(o)\n" \
        "    if busy or st.func~=f or st.proxy~=proxy or st.host~=host or st.guest~=guest or st.epoch~=epoch then return get(o) end\n" \
        "    busy=true\n" \
        "    local ok,p=pcall(update,o)\n" \
        "    busy=false\n" \
        "    if not ok then error(p,0) end\n" \
        "    return p\n" \
        "end\n" \
        "proxy=setmetatable({transform=setmetatable({GetTranslation=read},{__index=env.transform})},{__index=env,__newindex=env})\n" \
        "st.func=f; st.env=env; st.proxy=proxy\n" \
        "setfenv(f,proxy)\n" \
        "return report(1,-1)\n"

#define BETTER_TOGETHER_CAM_STOP_SCRIPT \
        "local st=rawget(_G,\"__bt_ext_cam\")\n" \
        "if type(st)==\"table\" then\n" \
        "    if st.look_stop then st.look_stop(); st.look_stop=nil end\n" \
        "    if st.func and st.env then setfenv(st.func,st.env) end\n" \
        "    st.func=nil; st.proxy=nil; st.env=nil; st.epoch=nil; st.probe=nil; st.probe_result=nil\n" \
        "    st.limit_state=0; st.limit_err=nil; st.limit_hits=0\n" \
        "    st.wave_state=0; st.wave_outside_since=nil; st.wave_pending=false; st.wave_last=nil\n" \
        "end\n"

#endif
