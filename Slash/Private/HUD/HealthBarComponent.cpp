//


#include "HUD/HealthBarComponent.h"
#include "HUD/HealthBar.h"
#include "Components/ProgressBar.h"

void UHealthBarComponent::SetHealthPercent(float Percent)
{
	// We make a variable and cast it only when its null. This way we will save on casting operations every tick when we check health
	if (HealthBarWidget == nullptr)
	{
		HealthBarWidget = Cast<UHealthBar>(GetUserWidgetObject());
	}
	

	if (HealthBarWidget && HealthBarWidget->HealthBar)
	{
		// Accesing UProgressBar component called HealthBar and using its function SetPercent passing in percent value from input
		HealthBarWidget->HealthBar->SetPercent(Percent);
	}

}
