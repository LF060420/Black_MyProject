#include "Treasure.h"
#include <Interfaces/PickUpInterface.h>
#include "Kismet/GameplayStatics.h"

//球体开始重叠事件
void ATreasure::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	IPickUpInterface* PickUpInterface = Cast<IPickUpInterface>(OtherActor);
	if (PickUpInterface)
	{
		PickUpInterface->AddGold(this);
		SpawnPickUpSound();
		Destroy();

		UE_LOG(LogTemp, Warning, TEXT("Treasure Overlap"));
	}

}
