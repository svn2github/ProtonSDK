#include "PlatformPrecomp.h"
 
#include "EntityUtils.h"

Entity * CreateOverlayEntity(Entity *pParentEnt, string name, string fileName, float x, float y)
{
	Entity *pEnt = pParentEnt->AddEntity(new Entity(name));
	if (!pEnt)
	{
		LogError("Failed creating entity");
		return NULL;
	}
	EntityComponent *pComp = pEnt->AddComponent(new OverlayRenderComponent());
	pComp->GetVar("fileName")->Set(fileName); //local to component
	pEnt->GetVar("pos2d")->Set(x,y);  //shared with whole entity

	return pEnt;
}

Entity * CreateOverlayButtonEntity(Entity *pParentEnt, string name, string fileName, float x, float y)
{
	
	Entity *pButtonEnt = CreateOverlayEntity(pParentEnt, name, fileName, x, y);
	pButtonEnt->AddComponent(new TouchHandlerComponent);
	pButtonEnt->AddComponent(new Button2DComponent);
	return pButtonEnt;
}

void SlideScreen(Entity *pEnt, bool bIn, int speedMS, int delayToStartMS)
{
	
	CL_Vec2f vEndPos;
	eInterpolateType interpolateType;
	CL_Vec2f vOrigPos = pEnt->GetVar("pos2d")->GetVector2();

	if (bIn)
	{
		//move it off screen to start
		pEnt->GetVar("pos2d")->Set(CL_Vec2f( float(-GetScreenSizeX()+vOrigPos.x), vOrigPos.y));
		vEndPos = CL_Vec2f(vOrigPos.x,vOrigPos.y);
		interpolateType = INTERPOLATE_SMOOTHSTEP;
	} else
	{
		pEnt->GetShared()->GetVarWithDefault("pos2d", CL_Vec2f(0,vOrigPos.y));

		vEndPos = CL_Vec2f(GetScreenSizeXf(),vOrigPos.y);
		interpolateType = INTERPOLATE_SMOOTHSTEP;
	}

	EntityComponent * pComp = pEnt->AddComponent( new InterpolateComponent);
	pComp->GetVar("var_name")->Set("pos2d");
	pComp->GetVar("target")->Set(vEndPos);
	pComp->GetVar("interpolation")->Set(uint32(interpolateType));
	pComp->GetVar("on_finish")->Set(uint32(InterpolateComponent::ON_FINISH_DIE));

	if (delayToStartMS == 0)
	{
		pComp->GetVar("duration_ms")->Set(uint32(speedMS));
	} else
	{
		//trigger it to start later
		GetMessageManager()->SetComponentVariable(pComp, delayToStartMS, "duration_ms", Variant(uint32(speedMS)));
	}
}

void SlideScreenVertical(Entity *pEnt, bool bIn, int speedMS, int delayToStartMS)
{

	CL_Vec2f vEndPos;
	eInterpolateType interpolateType;
	CL_Vec2f vOrigPos = pEnt->GetVar("pos2d")->GetVector2();

	if (bIn)
	{
		//move it off screen to start
		pEnt->GetVar("pos2d")->Set(CL_Vec2f( vOrigPos.x, -GetScreenSizeYf()));
		vEndPos = CL_Vec2f(vOrigPos.x,0);
		interpolateType = INTERPOLATE_SMOOTHSTEP;
	} else
	{
		pEnt->GetShared()->GetVarWithDefault("pos2d", CL_Vec2f(vOrigPos.x,0));

		vEndPos = CL_Vec2f(vOrigPos.x, GetScreenSizeYf());
		interpolateType = INTERPOLATE_SMOOTHSTEP;
	}

	EntityComponent * pComp = pEnt->AddComponent( new InterpolateComponent);
	pComp->GetVar("var_name")->Set("pos2d");
	pComp->GetVar("target")->Set(vEndPos);
	pComp->GetVar("interpolation")->Set(uint32(interpolateType));
	pComp->GetVar("on_finish")->Set(uint32(InterpolateComponent::ON_FINISH_DIE));

	if (delayToStartMS == 0)
	{
		pComp->GetVar("duration_ms")->Set(uint32(speedMS));
	} else
	{
		//trigger it to start later
		GetMessageManager()->SetComponentVariable(pComp, delayToStartMS, "duration_ms", Variant(uint32(speedMS)));
	}
}

//bounces for ever
void BobEntity(Entity *pEnt, float bobAmount)
{
	CL_Vec2f vEndPos = pEnt->GetVar("pos2d")->GetVector2();

	vEndPos.y += bobAmount;
	EntityComponent * pComp = pEnt->AddComponent( new InterpolateComponent);
	pComp->GetVar("var_name")->Set("pos2d");
	pComp->GetVar("target")->Set(vEndPos);
	pComp->GetVar("duration_ms")->Set(uint32(1000));
	pComp->GetVar("interpolation")->Set(uint32(INTERPOLATE_SMOOTHSTEP));
	pComp->GetVar("on_finish")->Set(uint32(InterpolateComponent::ON_FINISH_BOUNCE));
}

void OneTimeBobEntity(Entity *pEnt, float bobAmount, int delayBeforeBob, int durationMS)
{
	if (pEnt->GetComponentByName("ic_pos"))
	{
		//well, we already have one of these active, don't trigger it again yet until it's dead

	} else
	{

		EntityComponent *pComp;
		CL_Vec2f vEndPos = pEnt->GetVar("pos2d")->GetVector2();
		vEndPos.y += bobAmount;

		pComp = pEnt->AddComponent(new InterpolateComponent);
		pComp->SetName("ic_pos"); //so we can easily find this later, don't change this, apps rely on this
		pComp->GetVar("var_name")->Set("pos2d");
		pComp->GetVar("target")->Set(vEndPos);
		pComp->GetVar("interpolation")->Set(uint32(INTERPOLATE_SMOOTHSTEP));
		pComp->GetVar("on_finish")->Set(uint32(InterpolateComponent::ON_FINISH_BOUNCE));
		pComp->GetVar("deleteAfterPlayCount")->Set(uint32(2));
	
		if (delayBeforeBob == 0)
		{
			//do it now
			pComp->GetVar("duration_ms")->Set(uint32(durationMS));
		} else
		{
			//trigger it to start later
			GetMessageManager()->SetComponentVariable(pComp, delayBeforeBob, "duration_ms", Variant(uint32(durationMS)));
		}
	}
}

void AnimateStopEntity(Entity *pEnt, int delayToStartMS)
{
	EntityComponent *overlayComp = pEnt->GetComponentByName("OverlayRender");

	EntityComponent *pComp = pEnt->GetComponentByName("ic_anim");
	if (pComp)
	{
		if (delayToStartMS == 0)
		{
			pComp->GetVar("duration_ms")->Set(uint32(0));
		} else
		{
			GetMessageManager()->SetComponentVariable(pComp, delayToStartMS, "duration_ms", Variant(uint32(0)));
		}
	}
}

void AnimateStopEntityAndSetFrame(Entity *pEnt, int delayToStartMS, int frameX, int frameY)
{
	EntityComponent *overlayComp = pEnt->GetComponentByName("OverlayRender");

	EntityComponent *pComp = pEnt->GetComponentByName("ic_anim");
	if (pComp)
	{
		if (delayToStartMS == 0)
		{
			pComp->GetVar("duration_ms")->Set(uint32(0));

		} else
		{
			GetMessageManager()->SetComponentVariable(pComp, delayToStartMS, "duration_ms", Variant(uint32(0)));
		}
	}

	pComp = pEnt->GetComponentByName("OverlayRender");
	if (pComp)
	{
		if (delayToStartMS == 0)
		{
			pComp->GetVar("frameX")->Set(uint32(frameX));
			pComp->GetVar("frameY")->Set(uint32(frameY));
		} else
		{
			GetMessageManager()->SetComponentVariable(pComp, delayToStartMS, "frameX", Variant(uint32(frameX)));
			GetMessageManager()->SetComponentVariable(pComp, delayToStartMS, "frameY", Variant(uint32(frameY)));

		}

	}


}

EntityComponent * SetupAnimEntity(Entity *pEnt, uint32 frameCountX, uint32 frameCountY, int curFrameX, int curFrameY)
{
	EntityComponent *pComp = pEnt->GetComponentByName("OverlayRender");
	
	if (pEnt)
	{
		
		pComp->GetFunction("SetupAnim")->sig_function(&VariantList(frameCountX, frameCountY));

		if (curFrameX != -1)
		{
			//also set this..
			pComp->GetVar("frameX")->Set(uint32(curFrameX));
		} 
		
		if (curFrameY != -1)
		{
			pComp->GetVar("frameY")->Set(uint32(curFrameY));
		}

		return pComp; //return it just in case they want to do something else with it
	}

	assert("This anim doesn't even have an OverlayRender attached!");
	return NULL;
}

void AnimateEntitySetMirrorMode(Entity *pEnt, bool flipX, bool flipY)
{
	EntityComponent *overlayComp = pEnt->GetComponentByName("OverlayRender");
	assert(overlayComp && "You must add a OverlayRender component to use this");
	if (!overlayComp) return;

	overlayComp->GetVar("flipX")->Set(uint32(flipX));
	overlayComp->GetVar("flipY")->Set(uint32(flipY));

}

void AnimateEntity(Entity *pEnt, int startFrame, int endFrame, int animSpeedMS, InterpolateComponent::eOnFinish type, int delayToStartMS)
{
	EntityComponent *overlayComp = pEnt->GetComponentByName("OverlayRender");
	assert(overlayComp && "You must add a OverlayRender component to use this");
	if (!overlayComp) return;

	int totalFramesX = overlayComp->GetVar("totalFramesX")->GetUINT32();

	string frameName;

	if (totalFramesX > 1)
	{
		frameName = "frameX";
	} else
	{
		frameName = "frameY";
	}

	assert (type != InterpolateComponent::ON_FINISH_DIE && "You should use ON_FINISH_STOP so additional calls won't break stuff");
	EntityComponent *pComp = pEnt->GetComponentByName("ic_anim");
	if (!pComp)
	{
		//doesn't exist, create one
		pComp = pEnt->AddComponent(new InterpolateComponent);
		pComp->SetName("ic_anim");
	}

	uint32 totalTimeMS = animSpeedMS* (endFrame-startFrame);
	
	/*
	if (type == InterpolateComponent::ON_FINISH_BOUNCE)
	{
		totalTimeMS *= 2; //need twice as much
	}
	*/

	if (delayToStartMS == 0)
	{
		//set it all now
		pComp->GetVar("component_name")->Set("OverlayRender");
		pComp->GetVar("var_name")->Set(frameName);
		overlayComp->GetVar(frameName)->Set(uint32(startFrame));
		pComp->GetVar("target")->Set(uint32(endFrame+1));
		pComp->GetVar("interpolation")->Set(uint32(INTERPOLATE_LINEAR));
		pComp->GetVar("on_finish")->Set(uint32(type));
		pComp->GetVar("duration_ms")->Set(uint32(totalTimeMS));
	} else
	{
		//otherwise..schedule it so we can modify an anim on the fly by completely replacing it later
		GetMessageManager()->SetComponentVariable(pComp, delayToStartMS, "component_name", Variant("OverlayRender"));
		GetMessageManager()->SetComponentVariable(pComp, delayToStartMS, "var_name", Variant(frameName));
		GetMessageManager()->SetComponentVariable(overlayComp, delayToStartMS, frameName, Variant(uint32(startFrame)));
		GetMessageManager()->SetComponentVariable(pComp, delayToStartMS, "target", Variant(uint32(endFrame)+1));
		GetMessageManager()->SetComponentVariable(pComp, delayToStartMS, "interpolation", Variant(uint32(INTERPOLATE_LINEAR)));
		GetMessageManager()->SetComponentVariable(pComp, delayToStartMS, "on_finish", Variant(uint32(type)));
		GetMessageManager()->SetComponentVariable(pComp, delayToStartMS, "duration_ms", Variant(uint32(totalTimeMS)));
	}
	
}

void FadeInEntity(Entity *pEnt, bool bRecursive, int timeMS, int delayBeforeFadingMS)
{
	pEnt->GetVar("alpha")->Set(0.0f); //so we can fade in

	EntityComponent * pComp = pEnt->AddComponent( new InterpolateComponent);
	pComp->GetVar("var_name")->Set("alpha");
	pComp->GetVar("target")->Set(1.0f);
	pComp->GetVar("interpolation")->Set(uint32(INTERPOLATE_SMOOTHSTEP));
	pComp->GetVar("on_finish")->Set(uint32(InterpolateComponent::ON_FINISH_DIE));

	if (delayBeforeFadingMS == 0)
	{
		//do it now
		pComp->GetVar("duration_ms")->Set(uint32(timeMS));
	} else
	{
		//trigger it to start later
		GetMessageManager()->SetComponentVariable(pComp, delayBeforeFadingMS, "duration_ms", Variant(uint32(timeMS)), GetBaseApp()->GetActiveTimingSystem());
	}

	if (!bRecursive) return;

	//also run this on all children
	EntityList *pChildren = pEnt->GetChildren();

	EntityList::iterator itor = pChildren->begin();
	while (itor != pChildren->end())
	{
		FadeInEntity( *itor, bRecursive, timeMS, delayBeforeFadingMS);
		itor++;
	}

}

EntityComponent * ZoomToPositionFromThisOffsetEntity(Entity *pEnt, CL_Vec2f vPos, unsigned int speedMS, eInterpolateType interpolateType,  int delayBeforeActionMS)
{
	CL_Vec2f vEndPos;
	
	vEndPos = pEnt->GetVar("pos2d")->GetVector2();
	pEnt->GetVar("pos2d")->Set(vEndPos+vPos);
	EntityComponent * pComp = pEnt->GetComponentByName("ic_pos");

	if (!pComp)
	{
		pComp = pEnt->AddComponent( new InterpolateComponent);
		pComp->SetName("ic_pos");
	}
	pComp->GetVar("var_name")->Set("pos2d");
	pComp->GetVar("target")->Set(vEndPos);
	pComp->GetVar("interpolation")->Set(uint32(interpolateType));
	pComp->GetVar("on_finish")->Set(uint32(InterpolateComponent::ON_FINISH_DIE));

	if (delayBeforeActionMS == 0)
	{
		//do it now
		pComp->GetVar("duration_ms")->Set(uint32(speedMS));
	} else
	{
		//trigger it to start later
		GetMessageManager()->SetComponentVariable(pComp, delayBeforeActionMS, "duration_ms", Variant(uint32(speedMS)), GetBaseApp()->GetActiveTimingSystem());
	}


	return pComp;
}

EntityComponent * MorphToVec2Entity(Entity *pEnt, string targetVar, CL_Vec2f vTargetSize, unsigned int speedMS, eInterpolateType interpolateType,  int delayBeforeActionMS)
{
	EntityComponent * pComp = pEnt->GetComponentByName("ic_"+targetVar);

	if (!pComp)
	{
		pComp = pEnt->AddComponent( new InterpolateComponent);
		pComp->SetName("ic_"+targetVar);
	}
	pComp->GetVar("var_name")->Set(targetVar);
	pComp->GetVar("target")->Set(vTargetSize);
	pComp->GetVar("interpolation")->Set(uint32(interpolateType));
	pComp->GetVar("on_finish")->Set(uint32(InterpolateComponent::ON_FINISH_DIE));

	if (delayBeforeActionMS == 0)
	{
		//do it now
		pComp->GetVar("duration_ms")->Set(uint32(speedMS));
	} else
	{
		//trigger it to start later
		GetMessageManager()->SetComponentVariable(pComp, delayBeforeActionMS, "duration_ms", Variant(uint32(speedMS)), GetBaseApp()->GetActiveTimingSystem());
	}

	return pComp;
}

EntityComponent * MorphToFloatEntity(Entity *pEnt, string targetVar, float target, unsigned int speedMS, eInterpolateType interpolateType,  int delayBeforeActionMS)
{
	EntityComponent * pComp = pEnt->GetComponentByName("ic_"+targetVar);

	if (!pComp)
	{
		pComp = pEnt->AddComponent( new InterpolateComponent);
		pComp->SetName("ic_"+targetVar);
	}
	pComp->GetVar("var_name")->Set(targetVar);
	pComp->GetVar("target")->Set(target);
	pComp->GetVar("interpolation")->Set(uint32(interpolateType));
	pComp->GetVar("on_finish")->Set(uint32(InterpolateComponent::ON_FINISH_DIE));

	if (delayBeforeActionMS == 0)
	{
		//do it now
		pComp->GetVar("duration_ms")->Set(uint32(speedMS));
	} else
	{
		//trigger it to start later
		GetMessageManager()->SetComponentVariable(pComp, delayBeforeActionMS, "duration_ms", Variant(uint32(speedMS)), GetBaseApp()->GetActiveTimingSystem());
	}

	return pComp;
}

EntityComponent * MorphToSizeEntity(Entity *pEnt, CL_Vec2f vTargetSize, unsigned int speedMS, eInterpolateType interpolateType,  int delayBeforeActionMS)
{
	return MorphToVec2Entity(pEnt, "size2d", vTargetSize, speedMS, interpolateType, delayBeforeActionMS);
}


EntityComponent * MorphToFloatComponent(EntityComponent *pTargetComp, string targetVar, float target, unsigned int speedMS, eInterpolateType interpolateType,  int delayBeforeActionMS)
{
	EntityComponent * pComp = pTargetComp->GetParent()->GetComponentByName("ic_"+targetVar);

	if (!pComp)
	{
		pComp = pTargetComp->GetParent()->AddComponent( new InterpolateComponent);
		assert(!pTargetComp->GetName().empty() && "You should name the component to avoid confusion");

		pComp->GetVar("component_name")->Set(pTargetComp->GetName());
		pComp->SetName("ic_"+targetVar);
	}
	pComp->GetVar("var_name")->Set(targetVar);
	pComp->GetVar("target")->Set(target);
	pComp->GetVar("interpolation")->Set(uint32(interpolateType));
	pComp->GetVar("on_finish")->Set(uint32(InterpolateComponent::ON_FINISH_DIE));

	if (delayBeforeActionMS == 0)
	{
		//do it now
		pComp->GetVar("duration_ms")->Set(uint32(speedMS));
	} else
	{
		//trigger it to start later
		GetMessageManager()->SetComponentVariable(pComp, delayBeforeActionMS, "duration_ms", Variant(uint32(speedMS)), GetBaseApp()->GetActiveTimingSystem());
	}

	return pComp;
}

EntityComponent * ZoomFromPositionEntity(Entity *pEnt, CL_Vec2f vPos, unsigned int speedMS, eInterpolateType interpolateType,  int delayBeforeActionMS)
{
	CL_Vec2f vEndPos;

	vEndPos = pEnt->GetVar("pos2d")->GetVector2();
	pEnt->GetVar("pos2d")->Set(vPos);
	
	EntityComponent * pComp = pEnt->GetComponentByName("ic_pos");

	if (!pComp)
	{
		pComp = pEnt->AddComponent( new InterpolateComponent);
		pComp->SetName("ic_pos");
	}
	pComp->GetVar("var_name")->Set("pos2d");
	pComp->GetVar("target")->Set(vEndPos);
		pComp->GetVar("interpolation")->Set(uint32(interpolateType));
	pComp->GetVar("on_finish")->Set(uint32(InterpolateComponent::ON_FINISH_DIE));
	
	if (delayBeforeActionMS == 0)
	{
		//do it now
		pComp->GetVar("duration_ms")->Set(uint32(speedMS));
	} else
	{
		//trigger it to start later
		GetMessageManager()->SetComponentVariable(pComp, delayBeforeActionMS, "duration_ms", Variant(uint32(speedMS)), GetBaseApp()->GetActiveTimingSystem());
	}
	return pComp;
}

EntityComponent * ZoomToPositionEntity(Entity *pEnt, CL_Vec2f vPos, unsigned int speedMS, eInterpolateType interpolateType, int delayBeforeActionMS)
{
	return MorphToVec2Entity(pEnt, "pos2d", vPos, speedMS, interpolateType, delayBeforeActionMS);
}

EntityComponent * ZoomToPositionOffsetEntity(Entity *pEnt, CL_Vec2f vPos, unsigned int speedMS, eInterpolateType interpolateType, int delayBeforeActionMS)
{
	return MorphToVec2Entity(pEnt, "pos2d", pEnt->GetVar("pos2d")->GetVector2()+vPos, speedMS, interpolateType, delayBeforeActionMS);
}

EntityComponent * ZoomToPositionEntityMulti(Entity *pEnt, CL_Vec2f vPos, unsigned int speedMS, eInterpolateType interpolateType, int delayBeforeActionMS)
{
	EntityComponent * pComp = pEnt->AddComponent( new InterpolateComponent);
	pComp->SetName("ic_pos_multi");

	pComp->GetVar("var_name")->Set("pos2d");
	pComp->GetVar("target")->Set(vPos);
	pComp->GetVar("interpolation")->Set(uint32(interpolateType));
	pComp->GetVar("on_finish")->Set(uint32(InterpolateComponent::ON_FINISH_DIE));

	if (delayBeforeActionMS == 0)
	{
		//do it now
		pComp->GetVar("duration_ms")->Set(uint32(speedMS));
	} else
	{
		//trigger it to start later
		GetMessageManager()->SetComponentVariable(pComp, delayBeforeActionMS, "duration_ms", Variant(uint32(speedMS)), GetBaseApp()->GetActiveTimingSystem());
	}

	return pComp;
}


void MorphToColorEntity(Entity *pEnt, bool bRecursive, int timeMS, unsigned int color, int delayBeforeActionMS)
{
	
	EntityComponent * pComp = pEnt->GetComponentByName("ic_color");

	if (!pComp)
	{
		pComp = pEnt->AddComponent( new InterpolateComponent);
		pComp->SetName("ic_color");
		pComp->GetVar("var_name")->Set("color");
	}

	pComp->GetVar("target")->Set(uint32(color));
	pComp->GetVar("interpolation")->Set(uint32(INTERPOLATE_SMOOTHSTEP_AS_COLOR));
	pComp->GetVar("on_finish")->Set(uint32(InterpolateComponent::ON_FINISH_DIE));

	if (delayBeforeActionMS == 0)
	{
		//do it now
		pComp->GetVar("duration_ms")->Set(uint32(timeMS));
	} else
	{
		//trigger it to start later
		GetMessageManager()->SetComponentVariable(pComp, delayBeforeActionMS, "duration_ms", Variant(uint32(timeMS)), GetBaseApp()->GetActiveTimingSystem());
	}

	if (!bRecursive) return;

	//also run this on all children
	EntityList *pChildren = pEnt->GetChildren();

	EntityList::iterator itor = pChildren->begin();
	while (itor != pChildren->end())
	{
		MorphToColorEntity( *itor, bRecursive, timeMS, color, delayBeforeActionMS);
		itor++;
	}

}


EntityComponent * PulsateColorEntity(Entity *pEnt, bool bRecursive, unsigned int color)
{
	EntityComponent * pComp = pEnt->AddComponent( new InterpolateComponent);
	pComp->GetVar("var_name")->Set("colorMod");
	pComp->GetVar("target")->Set(uint32(color));
	pComp->GetVar("interpolation")->Set(uint32(INTERPOLATE_SMOOTHSTEP_AS_COLOR));
	pComp->GetVar("on_finish")->Set(uint32(InterpolateComponent::ON_FINISH_BOUNCE));
	pComp->GetVar("duration_ms")->Set(uint32(1000));

	if (!bRecursive) return pComp;

	//also run this on all children
	EntityList *pChildren = pEnt->GetChildren();

	EntityList::iterator itor = pChildren->begin();
	while (itor != pChildren->end())
	{
		PulsateColorEntity( *itor, bRecursive, color);
		itor++;
	}

	return pComp;

}

void DisableAllButtonsEntity(Entity *pEnt, bool bRecursive)
{
	EntityComponent * pComp = pEnt->GetComponentByName("Button2D");

	if (pComp)
	{
		pComp->GetVar("disabled")->Set(uint32(1));
	}

	if (!bRecursive) return;

	//also run this on all children
	EntityList *pChildren = pEnt->GetChildren();

	EntityList::iterator itor = pChildren->begin();
	while (itor != pChildren->end())
	{
		DisableAllButtonsEntity( *itor, bRecursive);
		itor++;
	}
}


void FlashStopEntity(Entity *pEnt)
{
	pEnt->RemoveComponentByName("ic_flash");
}

void FlashStartEntity(Entity *pEnt, int flashSpeedMS)
{
	FlashStopEntity(pEnt);
	EntityComponent * pComp = pEnt->AddComponent( new InterpolateComponent);
	pComp->SetName("ic_flash");
	pComp->GetVar("set_value_on_finish")->Set(pEnt->GetVar("alpha")->GetFloat()); //grab a copy of the current alpha, to restore it later
	pComp->GetVar("var_name")->Set("alpha"); //the var we will be interpolating
	//pEnt->GetVar("alpha")->Set(0.5f); //set the current alpha
	pComp->GetVar("target")->Set(float(1)); //how maximum position of the flashing
	pComp->GetVar("interpolation")->Set(uint32(INTERPOLATE_SMOOTHSTEP));
	pComp->GetVar("on_finish")->Set(uint32(InterpolateComponent::ON_FINISH_BOUNCE));
	pComp->GetVar("duration_ms")->Set(uint32(flashSpeedMS/2));
}

void FlashOnceEntity(Entity *pEnt, int flashSpeedMS)
{
	pEnt->RemoveComponentByName("ic_flash");
	EntityComponent * pComp = pEnt->AddComponent( new InterpolateComponent);
	pComp->SetName("ic_flash");
	pComp->GetVar("set_value_on_finish")->Set(pEnt->GetVar("alpha")->GetFloat()); //grab a copy of the current alpha, to restore it later
	pComp->GetVar("var_name")->Set("alpha"); //the var we will be interpolating
	pComp->GetVar("target")->Set(float(1)); //how maximum position of the flashing
	pComp->GetVar("interpolation")->Set(uint32(INTERPOLATE_SMOOTHSTEP));
	pComp->GetVar("deleteAfterPlayCount")->Set(uint32(2));
	pComp->GetVar("on_finish")->Set(uint32(InterpolateComponent::ON_FINISH_BOUNCE));
	pComp->GetVar("duration_ms")->Set(uint32(flashSpeedMS/2));
}

void FadeEntity(Entity *pEnt, bool bRecursive, float alpha, int timeMS, int delayBeforeFadingMS,  bool bAllowMultipleFadesActiveAtOnce)
{
	if (!bAllowMultipleFadesActiveAtOnce)
	{
		pEnt->RemoveComponentByName("ic_fade");

	}
	EntityComponent * pComp = pEnt->AddComponent( new InterpolateComponent);
	pComp->SetName("ic_fade");
	pComp->GetVar("var_name")->Set("alpha");
	pComp->GetVar("target")->Set(alpha);
	pComp->GetVar("interpolation")->Set(uint32(INTERPOLATE_SMOOTHSTEP));
	pComp->GetVar("on_finish")->Set(uint32(InterpolateComponent::ON_FINISH_DIE));

	if (delayBeforeFadingMS == 0)
	{
		//do it now
		pComp->GetVar("duration_ms")->Set(uint32(timeMS));
	} else
	{
		//trigger it to start later
		GetMessageManager()->SetComponentVariable(pComp, delayBeforeFadingMS, "duration_ms", Variant(uint32(timeMS)));
	}

	if (!bRecursive) return;

	//also run this on all children
	EntityList *pChildren = pEnt->GetChildren();

	EntityList::iterator itor = pChildren->begin();
	while (itor != pChildren->end())
	{
		FadeEntity( *itor, bRecursive, alpha, timeMS, delayBeforeFadingMS);
		itor++;
	}
}

void FadeOutEntity(Entity *pEnt, bool bRecursive, int timeMS, int delayBeforeFadingMS)
{

	EntityComponent * pComp = pEnt->AddComponent( new InterpolateComponent);
	pComp->GetVar("var_name")->Set("alpha");
	pComp->GetVar("target")->Set(0.0f);
	pComp->GetVar("interpolation")->Set(uint32(INTERPOLATE_SMOOTHSTEP));
	pComp->GetVar("on_finish")->Set(uint32(InterpolateComponent::ON_FINISH_DIE));

	if (delayBeforeFadingMS == 0)
	{
		//do it now
		pComp->GetVar("duration_ms")->Set(uint32(timeMS));
	} else
	{
		//trigger it to start later
		GetMessageManager()->SetComponentVariable(pComp, delayBeforeFadingMS, "duration_ms", Variant(uint32(timeMS)));
	}

	if (!bRecursive) return;

	//also run this on all children
	EntityList *pChildren = pEnt->GetChildren();

	EntityList::iterator itor = pChildren->begin();
	while (itor != pChildren->end())
	{
		FadeOutEntity( *itor, bRecursive, timeMS, delayBeforeFadingMS);
		itor++;
	}
}

void FadeOutAndKillEntity(Entity *pEnt, bool bRecursive, int timeMS, int delayBeforeFadingMS)
{
	if (!pEnt) return;
	FadeOutEntity(pEnt, bRecursive, timeMS, delayBeforeFadingMS);
	KillEntity(pEnt, timeMS+delayBeforeFadingMS);
}

void FadeOutAndKillChildrenEntities(Entity *pEnt, int timeMS, int delayBeforeFadingMS)
{
	EntityList *pChildren = pEnt->GetChildren();

	EntityList::iterator itor = pChildren->begin();
	while (itor != pChildren->end())
	{
		FadeOutEntity( *itor, true, timeMS, delayBeforeFadingMS);
		KillEntity(*itor, timeMS+delayBeforeFadingMS);
		itor++;
	}
}


void ScaleEntity(Entity *pEnt, float scaleStart, float scaleEnd, int timeMS, int delayBeforeStartingMS)
{
	pEnt->RemoveComponentByName("ic_scale");

	EntityComponent * pComp = pEnt->AddComponent( new InterpolateComponent);
	pComp->SetName("ic_scale");
	pComp->GetVar("var_name")->Set("scale2d");
	if (scaleStart != -1)
	{
		pEnt->GetVar("scale2d")->Set(CL_Vec2f(scaleStart, scaleStart));
	}
	pComp->GetVar("target")->Set(CL_Vec2f(scaleEnd, scaleEnd));
	pComp->GetVar("interpolation")->Set(uint32(INTERPOLATE_SMOOTHSTEP));
	pComp->GetVar("on_finish")->Set(uint32(InterpolateComponent::ON_FINISH_DIE));

	if (delayBeforeStartingMS == 0)
	{
		//do it now
		pComp->GetVar("duration_ms")->Set(uint32(timeMS));
	} else
	{
		//trigger it to start later
		GetMessageManager()->SetComponentVariable(pComp, delayBeforeStartingMS, "duration_ms", Variant(uint32(timeMS)));
	}

}

void KillEntity(Entity *pEnt, int timeMS)
{
	if (!pEnt) return;

	if (timeMS == 0)
	{
		pEnt->SetTaggedForDeletion();
	} else
	{
		GetMessageManager()->CallEntityFunction(pEnt, timeMS, "OnDelete", &VariantList(pEnt));
	}
}


Entity * CreateTextLabelEntity(Entity *pParentEnt, string name, float x, float y, string text)
{
	Entity *pButtonEnt = pParentEnt->AddEntity(new Entity(name));
	EntityComponent *pComp  = pButtonEnt->AddComponent(new TextRenderComponent());
	pComp->GetVar("text")->Set(text); //local to component
	pButtonEnt->GetVar("pos2d")->Set(x, y);
	return pButtonEnt;
}


Entity * CreateInputTextEntity(Entity *pParentEnt, string name, float x, float y, string text, float sizeX, float sizeY)
{
	
	Entity *pButtonEnt = pParentEnt->AddEntity(new Entity(name));
	EntityComponent *pComp  = pButtonEnt->AddComponent(new InputTextRenderComponent());
	pButtonEnt->AddComponent( new TouchHandlerComponent);
	pComp->GetVar("text")->Set(text); //local to component
	pButtonEnt->GetVar("pos2d")->Set(x, y);
	float fontHeight = GetBaseApp()->GetFont(FONT_SMALL)->GetLineHeight(1.0f);
	if (sizeX == 0) sizeX = fontHeight*10;
	if (sizeY == 0) sizeY = fontHeight+6;

	pButtonEnt->GetVar("size2d")->Set(sizeX, sizeY);
	return pButtonEnt;
}

Entity * CreateOverlayRectEntity(Entity *pParent, CL_Rectf posAndBoundsRect, uint32 color, RectRenderComponent::eVisualStyle style)
{
	Entity *pEnt = pParent->AddEntity(new Entity);
	EntityComponent *pComp = pEnt->AddComponent(new RectRenderComponent);
	pEnt->GetVar("pos2d")->Set(posAndBoundsRect.get_top_left());
	pEnt->GetVar("size2d")->Set(posAndBoundsRect.get_width(), posAndBoundsRect.get_height());
	pEnt->GetVar("color")->Set(color);
	if (style != RectRenderComponent::STYLE_NORMAL)
	{
		pComp->GetVar("visualStyle")->Set(uint32(style));
	}
	return pEnt;
};

Entity * CreateOverlayRectEntity(Entity *pParent, CL_Vec2f vPos, CL_Vec2f vBounds, uint32 color, RectRenderComponent::eVisualStyle style)
{
	return CreateOverlayRectEntity(pParent, CL_Rectf(vPos, *((CL_Sizef*)&vBounds) ), color, style);
};


Entity * CreateButtonHotspot(Entity *pParentEnt, string name, CL_Vec2f vPos, CL_Vec2f vBounds, Button2DComponent::eButtonStyle buttonStyle)
{
	Entity *pButtonEnt = CreateOverlayRectEntity(pParentEnt, vPos, vBounds, MAKE_RGBA(0,0,0,100));
	pButtonEnt->SetName(name);
	pButtonEnt->AddComponent(new TouchHandlerComponent);
	pButtonEnt->GetVar("touchPadding")->Set(CL_Rectf(0,0,0,0));

	EntityComponent *pButtonComp = pButtonEnt->AddComponent(new Button2DComponent);
	//pButtonEnt->GetVar("alpha")->Set(0.0f);
	//don't shot the hotspot rect
	pButtonComp->GetVar("buttonStyle")->Set(uint32(buttonStyle));
	pButtonComp->GetVar("visualStyle")->Set(uint32(Button2DComponent::STYLE_INVISIBLE_UNTIL_CLICKED));
	return pButtonEnt;
}

void SetButtonRepeatDelayMS(Entity *pEnt, uint32 delayMS)
{
	EntityComponent *pBut = pEnt->GetComponentByName("Button2D");
	if (pBut)
	{
		pBut->GetVar("repeatDelayMS")->Set(delayMS);
	} else
	{
		assert(!"Entity doesn't have a button component, so why call this?");
	}
}

void SetButtonClickSound(Entity *pEnt, string fileName)
{
	EntityComponent *pBut = pEnt->GetComponentByName("Button2D");
	if (pBut)
	{
		pBut->GetVar("onClickAudioFile")->Set(fileName);
	} else
	{
		assert(!"Entity doesn't have a button component, so why call this?");
	}
}

Entity * CreateTextButtonEntity(Entity *pParentEnt, string name, float x, float y, string text, bool bUnderline)
{
	Entity *pButtonEnt = CreateTextLabelEntity(pParentEnt, name, x, y, text);
	pButtonEnt->AddComponent(new TouchHandlerComponent);
	pButtonEnt->AddComponent(new Button2DComponent);
	if (bUnderline)
	{
		pButtonEnt->AddComponent(new UnderlineRenderComponent);
	}

	return pButtonEnt;
}


void RemoveFocusIfNeeded(Entity *pEnt)
{
	pEnt->RemoveComponentByName("FocusInput");
	pEnt->RemoveComponentByName("FocusRender");
	pEnt->RemoveComponentByName("FocusUpdate");
}

void AddFocusIfNeeded(Entity *pEnt, bool bAlsoLinkMoveMessages, int delayInputMS)
{
	if (!pEnt->GetComponentByName("FocusUpdate", true))
		pEnt->AddComponent(new FocusUpdateComponent);

	if (!pEnt->GetComponentByName("FocusRender", true))
		pEnt->AddComponent(new FocusRenderComponent);

	if (!pEnt->GetComponentByName("FocusInput", true))
	{
		if (delayInputMS == 0)
		{
			FocusInputComponent *pComp = (FocusInputComponent*)pEnt->AddComponent(new FocusInputComponent);
		
			if (bAlsoLinkMoveMessages)
			{
				pComp->GetFunction("LinkMoveMessages")->sig_function(NULL);
			}
		} else
		{
			//add the input focus, but wait a bit before doing it
			GetMessageManager()->AddComponent(pEnt, delayInputMS, new FocusInputComponent);
			//call a function on a component that doesn't exist yet, but will be added in 500 ms
			GetMessageManager()->CallComponentFunction(pEnt, "FocusInput", delayInputMS, "LinkMoveMessages");

		}
	}
}

//adds input focus, but ONLY for touch movement, nothing else.  Useful if you're already receiving the other inputs by trickle down from
//another focus.

void AddInputMovementFocusIfNeeded(Entity *pEnt)
{
	if (!pEnt->GetComponentByName("FocusInput", false))
	{
			FocusInputComponent *pComp = new FocusInputComponent;
			
			//tell it not to wire anything
			pComp->GetVar("mode")->Set(uint32(FocusInputComponent::MODE_START_NONE));
			pEnt->AddComponent(pComp);
			pComp->GetFunction("LinkMoveMessages")->sig_function(NULL);
	}
}

void FadeScreen( Entity *pParent, float defaultStartAlpha, float targetAlpha, int fadeDurationMS, bool bDeleteWhenDone )
{
	
	Entity *pEnt = pParent->GetEntityByName("black_overlay");
	
	if (!pEnt)
	{
		pEnt = pParent->AddEntity(new Entity("black_overlay"));
		pEnt->AddComponent(new RectRenderComponent);
		pEnt->GetVar("size2d")->Set(CL_Vec2f(GetScreenSizeXf(), GetScreenSizeYf()));
		pEnt->GetVar("color")->Set(MAKE_RGBA(0,0,0,255));
		pEnt->GetVar("alpha")->Set(defaultStartAlpha);
	}

	FadeEntity(pEnt, false, targetAlpha, fadeDurationMS, 0);

	if (bDeleteWhenDone)
	{
		pEnt->SetName("ic_delete"); //safety measure so if they call FadeScreen again fast this soon to be deleted versuion won't confuse it
		KillEntity(pEnt, fadeDurationMS);
	}
}

void FadeScreenUp( Entity *pParent, float targetAlpha, int fadeDurationMS, bool bDeleteWhenDone )
{

	Entity *pEnt = pParent->GetEntityByName("black_overlay");

	if (!pEnt)
	{
		pEnt = pParent->AddEntity(new Entity("black_overlay"));
		pEnt->AddComponent(new RectRenderComponent);
		pEnt->GetVar("size2d")->Set(CL_Vec2f(GetScreenSizeXf(), GetScreenSizeYf()));
		pEnt->GetVar("color")->Set(MAKE_RGBA(0,0,0,255));
		pEnt->GetVar("alpha")->Set(1.0f);
	}

	FadeEntity(pEnt, false, targetAlpha, fadeDurationMS, 0);

	if (bDeleteWhenDone)
	{
		KillEntity(pEnt, fadeDurationMS);
	}
}



CL_Rectf MeasureEntityAndChildren(Entity *pEnt, bool bFirst)
{
	CL_Vec2f vSize = pEnt->GetVar("size2d")->GetVector2();
	CL_Vec2f vPos = pEnt->GetVar("pos2d")->GetVector2();
	
	eAlignment align = eAlignment(pEnt->GetVar("alignment")->GetUINT32());
	if (align != ALIGNMENT_UPPER_LEFT)
	{
		vPos -= GetAlignmentOffset(vSize, align);
	}

	
	CL_Rectf r = CL_Rectf(0,0,vSize.x, vSize.y);
	if (!bFirst)
	r.translate(vPos.x, vPos.y);

	//also run this on all children
	EntityList *pChildren = pEnt->GetChildren();

	EntityList::iterator itor = pChildren->begin();
	while (itor != pChildren->end())
	{
		CL_Rectf childR = MeasureEntityAndChildren(*itor, false);

		r.bounding_rect(childR);
		itor++;
	}

	return r;
}

void SetupTextEntity(Entity *pEnt, eFont fontID, float scale)
{
	EntityComponent *pComp = pEnt->GetComponentByName("TextRender");
	if (!pComp)
	{
		assert(!"Huh?");
		return;
	}

	if (scale != 0)
	{
		pEnt->GetVar("scale2d")->Set(CL_Vec2f(scale, scale));
	}

	pComp->GetVar("font")->Set(uint32(fontID));
}

void SetAlignmentEntity(Entity *pEnt, eAlignment align)
{
	pEnt->GetVar("alignment")->Set(uint32(align));
}

Entity * CreateTextBoxEntity(Entity *pParent, string entName, CL_Vec2f vPos, CL_Vec2f vTextAreaSize, string msg, float scale)
{
	Entity *pText = pParent->AddEntity(new Entity(entName));
	
	/*
	EntityComponent *pClip = pText->AddComponent(new RenderClipComponent);
	pClip->GetVar("clipMode")->Set(uint32(RenderClipComponent::CLIP_MODE_BOTTOM));

	*/

	EntityComponent *pTextComp = pText->AddComponent(new TextBoxRenderComponent);
	pText->GetVar("size2d")->Set(vTextAreaSize);
	pTextComp->GetVar("fontScale")->Set(scale);
	pTextComp->GetVar("text")->Set(msg);
	pText->GetVar("pos2d")->Set(vPos);
	return pText;
}

void SetTouchPaddingEntity( Entity *pEnt, CL_Rectf padding )
{
	pEnt->GetVar("touchPadding")->Set(padding);
}

EntityComponent * SetButtonVisualStyleEntity(Entity *pEnt, Button2DComponent::eVisualStyle style)
{
	EntityComponent *pComp = pEnt->GetComponentByName("Button2D");
	if (!pComp)
	{
		assert(!"Huh?");
		return NULL;
	}

	pComp->GetVar("visualStyle")->Set(uint32(style));
	return pComp;
}


EntityComponent * SetButtonStyleEntity(Entity *pEnt, Button2DComponent::eButtonStyle style)
{
	EntityComponent *pComp = pEnt->GetComponentByName("Button2D");
	if (!pComp)
	{
		assert(!"Huh?");
		return NULL;
	}

	pComp->GetVar("buttonStyle")->Set(uint32(style));
	return pComp;
}

EntityComponent * TypeTextLabelEntity( Entity *pEnt, int delayBeforeActionMS, uint32 textTypeSpeedMS)
{
	EntityComponent *pText = pEnt->GetComponentByName("TextRender");
	
	if (!pText)
	{
		pText = pEnt->GetComponentByName("TextBoxRender");
	}

	if (!pText)
	{
		assert("!This only works on an entity that has a TextRender or TextBoxRender component already!");
		return NULL;
	}

	EntityComponent *pTextTyper = pEnt->GetComponentByName("Typer");
	if (pTextTyper) pEnt->RemoveComponentByAddress(pTextTyper); //kill any existing ones
 
	pTextTyper = pEnt->AddComponent(new TyperComponent);
	pTextTyper->GetVar("mode")->Set(uint32(TyperComponent::MODE_ONCE_AND_REMOVE_SELF));
	pTextTyper->GetVar("speedMS")->Set(textTypeSpeedMS);


	string msg = pText->GetVar("text")->GetString();
	pText->GetVar("text")->Set(""); //clear what was there, it will be added by the text typer
	
	pTextTyper->GetVar("text")->Set(msg);
	
	if (delayBeforeActionMS != 0)
	{
		pTextTyper->GetVar("paused")->Set(uint32(1));
		GetMessageManager()->SetComponentVariable(pTextTyper, delayBeforeActionMS, "paused", Variant(uint32(0)));
	}
	return pTextTyper; //just in case they want to make more tweaks

}

void SetAlphaEntity( Entity *pEnt, float alpha )
{
	pEnt->GetVar("alpha")->Set(alpha);
}

void PreloadKeyboard(OSMessage::eParmKeyboardType keyboardType)
{
	if (GetEmulatedPlatformID() == PLATFORM_ID_ANDROID) return; //no point on this platform I don't think..

	OSMessage o;
	o.m_type = OSMessage::MESSAGE_OPEN_TEXT_BOX;
	o.m_string = "";
	SetLastStringInput("");
	o.m_x = -1000;
	o.m_y = -1000; 
	o.m_parm1 = 0;
	o.m_fontSize = 30.0f;
	o.m_sizeX = 217;
	o.m_sizeY = 40;
	o.m_parm2 = keyboardType;
	GetBaseApp()->AddOSMessage(o);

	o.m_type = OSMessage::MESSAGE_CLOSE_TEXT_BOX;
	GetBaseApp()->AddOSMessage(o);
}


void SendFakeInputMessageToEntity(Entity *pEnt, eMessageType msg, CL_Vec2f vClickPos)
{
	VariantList v;
	v.Get(0).Set((float)msg);
	v.Get(1).Set(vClickPos);
	pEnt->CallFunctionRecursively("OnInput", &v);
}

void LightBarOnChange(VariantList *pVList)
{
	Entity *pEnt = pVList->Get(1).GetEntity();
	//LogMsg("Clicked %s", pEnt->GetName().c_str());
	string barName = pEnt->GetName().substr(0, pEnt->GetName().rfind("_")) + "_lightbar";
	//LogMsg("Bar name is %s", barName.c_str());

	Entity *pLightBar = pEnt->GetParent()->GetEntityByName(barName);
	if (!pLightBar)
	{
		assert(0);
		return;
	}

	CL_Vec2f vOffset(4,2);
	ZoomToPositionEntity(pLightBar, pEnt->GetVar("pos2d")->GetVector2()-vOffset, 300);
	MorphToSizeEntity(pLightBar, pEnt->GetVar("size2d")->GetVector2()+(vOffset*2), 300);	
}

void SetupLightBarSelect(Entity *pBG, string entNamePrefix, int defaultOption, uint32 color)
{
	//count how many buttons we can find

	int buttonCount = 0;
	Entity *pEnt = NULL;
	Entity *pHighlighted = NULL;

	for (int i=0;;i++)
	{
		pEnt = pBG->GetEntityByName(entNamePrefix+toString(i));

		if (pEnt)
		{
			if (defaultOption == buttonCount)
			{
				pHighlighted = pEnt;
			}

			SetButtonStyleEntity(pEnt, Button2DComponent::BUTTON_STYLE_CLICK_ON_TOUCH);
			//map buttons so we know when it is clicked
			pEnt->GetFunction("OnButtonSelected")->sig_function.connect(&LightBarOnChange);

			buttonCount++;
		} else
		{
			break;
		}
	}

	//LogMsg("Found %d buttons.", buttonCount);

	pEnt = CreateOverlayRectEntity(pBG, CL_Vec2f(0,0), CL_Vec2f(30, 10), color);
	pEnt->SetName(entNamePrefix+"lightbar");

	assert(pHighlighted && "Need to specify the default");

	if  (pHighlighted)
	{
		CL_Vec2f vOffset(4,2);
		pEnt->GetVar("pos2d")->Set(pHighlighted->GetVar("pos2d")->GetVector2()-vOffset);
		pEnt->GetVar("size2d")->Set(pHighlighted->GetVar("size2d")->GetVector2()+(vOffset*2));
	}

	pBG->MoveEntityToBottomByAddress(pEnt); //let this draw first
}

//assumes you've setup a scroll window the way I always do... see examples.
void ResizeScrollBounds(VariantList *pVList)
{
	Entity *pScroll = pVList->Get(0).GetEntity()->GetEntityByName("scroll");
	Entity *pScrollChild = pScroll->GetEntityByName("scroll_child");
	if (!pScroll || !pScrollChild)
	{
		LogError("huh");
		return;
	}

	CL_Vec2f scrollSize = pScroll->GetVar("size2d")->GetVector2();

	CL_Rectf r = MeasureEntityAndChildren(pScrollChild);
	//	LogMsg("Resizing bounds to %s", PrintRect(r).c_str());
	CL_Rectf scrollRect = CL_Rectf( rt_min(0, (-r.get_width())+scrollSize.x), (-r.get_height())+scrollSize.y, 0, 0);

	pScroll->GetComponentByName("Scroll")->GetVar("boundsRect")->Set(scrollRect);
}

void DisableHorizontalScrolling(Entity *pEnt)
{
	Entity *pScroll = pEnt->GetEntityByName("scroll");
    if (!pScroll)
	{
		assert(!"This only works for an entity holding a ScrollComponent!");
		return;
	}

	pEnt->GetComponentByName("Scroll")->GetVar("boundsRect")->GetRect().left = 0;

}


EntityComponent * DisableComponentByName(Entity *pEnt, const string &compName, int delayBeforeActionMS)
{
	EntityComponent *pComp = pEnt->GetComponentByName(compName);
	if (!pComp)
	{
		assert("!Unable to find component");
		return NULL;
	}

	if (delayBeforeActionMS != 0)
	{
		GetMessageManager()->SetComponentVariable(pComp, delayBeforeActionMS, "disabled", Variant(uint32(1)));
	} else
	{
		//set it now
		pComp->GetVar("disabled")->Set(uint32(1));
	}

	return pComp;
}

EntityComponent * EnableComponentByName(Entity *pEnt, const string &compName, int delayBeforeActionMS)
{
	EntityComponent *pComp = pEnt->GetComponentByName(compName);
	if (!pComp)
	{
		assert("!Unable to find component");
		return NULL;
	}

	if (delayBeforeActionMS != 0)
	{
		GetMessageManager()->SetComponentVariable(pComp, delayBeforeActionMS, "disabled", Variant(uint32(0)));
	} else
	{
		//set it now
		pComp->GetVar("disabled")->Set(uint32(0));
	}

	return pComp;
}

CL_Vec2f ConvertEntityClickToScreenCoords(CL_Vec2f pt, Entity *pEnt)
{
	//remove any offsets/adjustments and figure out the true screen coordinates of a click

	eAlignment align = eAlignment(pEnt->GetVar("alignment")->GetUINT32());
	if (align != ALIGNMENT_UPPER_LEFT)
	{
		pt -= GetAlignmentOffset(pEnt->GetVar("size2d")->GetVector2(), align);
	}

	//also add parents positions?
	assert(pEnt->GetParent()->GetVar("pos2d")->GetVector2() == CL_Vec2f(0,0) && "Shouldn't you take this into account too?");

	return pt;
}

void GetUsedTextures(vector<string> &usedTextures, Entity *pEnt)
{

	//check components for anything we recognize that would use textures

	ComponentList *pComponents = pEnt->GetComponents();

	ComponentList::iterator compItor = pComponents->begin();
	while (compItor != pComponents->end())
	{
		EntityComponent *pComp = *compItor;

		if (pComp->GetName() == "OverlayRender")
		{
			string fName = pComp->GetVar("fileName")->GetString();

			if (!fName.empty()) usedTextures.push_back(fName);
		} else
		if (pComp->GetName() == "ScrollBarRender")
		{
			string fName = pComp->GetVar("fileName")->GetString();

			if (!fName.empty()) usedTextures.push_back(fName);
		}

		compItor++;
	}

	//also run this on all children
	EntityList *pChildren = pEnt->GetChildren();

	EntityList::iterator itor = pChildren->begin();
	while (itor != pChildren->end())
	{
		GetUsedTextures( usedTextures, *itor);
		itor++;
	}
}


void DestroyUnusedTextures()
{
	//first, make a list of everything that is used

	vector<string> usedTextures;

	GetUsedTextures(usedTextures, GetEntityRoot());
#ifdef _DEBUG
	LogMsg("Destroying unused textures");
#endif
	/*
	
	for (int i=0; i < usedTextures.size(); i++)
	{
	LogMsg("%d - %s", i, usedTextures[i].c_str());
	}
	
	*/

	GetBaseApp()->GetResourceManager()->RemoveTexturesNotInExclusionList(usedTextures);
}

bool EntityRetinaRemapIfNeeded(Entity *pEnt, bool bAdjustPosition, bool bAdjustScale,  bool bApplyToIpadAlso, bool bPerserveAspectRatio)
{
	/*
	LogMsg("Screen size is %s", PrintVector2(GetScreenSize()).c_str());
	
	if (IsIphone4Size) LogMsg("Is iphone4 sized");
	if (IsIPADSize) LogMsg("Is ipad sized");
	if (IsPixiSize) LogMsg("is pixi sized");
	*/

	if (!IsIphone4Size && !(bApplyToIpadAlso && IsIPADSize) && !IsPixiSize) return false;
	
	if (bAdjustPosition)
	{
		CL_Vec2f vPos = iPhoneMap(pEnt->GetVar("pos2d")->GetVector2());
		pEnt->GetVar("pos2d")->Set(vPos);
	}

	if (bAdjustScale)
	{
		CL_Vec2f vScale;

		if (!IsPixiSize)
		{
			vScale = pEnt->GetVar("scale2d")->GetVector2() * 2.0f;
		} else
		{
			if (bPerserveAspectRatio)
			{
				float scale = rt_min(GetScreenSizeXf()/480, GetScreenSizeYf()/320);
				vScale = CL_Vec2f(scale,scale);

			} else
			{
				vScale = CL_Vec2f(GetScreenSizeXf()/480, GetScreenSizeYf()/320);
			}
		}
		pEnt->GetVar("scale2d")->Set(vScale);

	}
	return true;
}

void EntitySetScaleBySize(Entity *pEnt, CL_Vec2f vDestSize)
{
	CL_Vec2f vSize = pEnt->GetVar("size2d")->GetVector2();
	assert(vSize.x != 0 && vSize.y != 0);

	if (vSize.x == 0 || vSize.y == 0)
	{
		assert(!"Huh?");
		return; //avoid divide by 0
	}
	pEnt->GetVar("scale2d")->Set(CL_Vec2f( vDestSize.x / vSize.x, vDestSize.y / vSize.y));

}

EntityComponent * AddHotKeyToButton(Entity *pEnt, uint32 keycode)
{
	if (!pEnt)
	{
		assert(!"Serious error");
		return NULL;
	}
	EntityComponent *pComp = pEnt->AddComponent(new SelectButtonWithCustomInputComponent);
	pComp->GetVar("keycode")->Set(keycode);
	return pComp;
}

EntityComponent * MakeButtonEmitVirtualGameKey(Entity *pEnt, uint32 keycode)
{
	if (!pEnt)
	{
		assert(!"Serious error");
		return NULL;
	}
	EntityComponent *pComp = pEnt->AddComponent(new EmitVirtualKeyComponent);
	pComp->GetVar("keycode")->Set(keycode);
	return pComp;
}

EntityComponent * CreateSlider(Entity *pBG, float x, float y, float sizeX, string buttonFileName, string left, string middle, string right)
{
	//first the BG
	CreateOverlayRectEntity(pBG, CL_Vec2f(x, y), CL_Vec2f(sizeX,3), MAKE_RGBA(255,255,255,255));

	//the text descriptions

	float textY = y- (GetBaseApp()->GetFont(FONT_SMALL)->GetLineHeight(1)+iPhoneMapY2X(15));

	CreateTextLabelEntity(pBG, "txt", x, textY, left);

	Entity *pText = CreateTextLabelEntity(pBG, "txt", x+sizeX/2, textY, middle);
	SetAlignmentEntity(pText, ALIGNMENT_UPPER_CENTER);

	pText = CreateTextLabelEntity(pBG, "txt", x+sizeX, textY, right);
	SetAlignmentEntity(pText, ALIGNMENT_UPPER_RIGHT);

	//then the slider control
	Entity *pSliderEnt = pBG->AddEntity(new Entity("SliderEnt"));
	EntityComponent * pSliderComp = pSliderEnt->AddComponent(new SliderComponent);

	//the button we move around to slide
	Entity *pSliderButton = CreateOverlayButtonEntity(pSliderEnt, "sliderButton",  buttonFileName, 0, 6);

	CL_Vec2f vButtonScale = CL_Vec2f(0.7f, 0.7f);

	if (IsLargeScreen())
	{
		vButtonScale = CL_Vec2f(1, 1);
	}
	
	pSliderButton->GetVar("scale2d")->Set(vButtonScale);
	pSliderButton->GetComponentByName("Button2D")->GetVar("onClickAudioFile")->Set("");

	CL_Vec2f vImageSize = pSliderButton->GetVar("size2d")->GetVector2();
	pSliderEnt->GetVar("pos2d")->Set(CL_Vec2f(x+(vImageSize.x/2)*0.5f, y));
	pSliderEnt->GetVar("size2d")->Set(CL_Vec2f(sizeX- (vImageSize.x*0.5f),0));

	//SetTouchPaddingEntity(pSliderButton, CL_Rectf(0,0,0,0));
	SetAlignmentEntity(pSliderButton, ALIGNMENT_CENTER);
	pSliderComp->GetVar("sliderButton")->Set(pSliderButton);
	return pSliderComp;
};

