#include "Grid.h"
#include "DrawDebugHelpers.h"

UGrid::UGrid()
{
	PrimaryComponentTick.bCanEverTick = true;
}


FVector UGrid::GetGridOriginOffset() const
{
	const FVector TotalSize(
		GridSize.X * CellSize.X,
		GridSize.Y * CellSize.Y,
		0.f
	);

	switch (GridOrigin)
	{
	case EGridOrigin::BottomLeft:
		return FVector(0.f, 0.f, 0.f);
	case EGridOrigin::BottomRight:
		return FVector(-TotalSize.X, 0.f, 0.f);
	case EGridOrigin::TopLeft:
		return FVector(0.f, -TotalSize.Y, 0.f);
	case EGridOrigin::TopRight:
		return FVector(-TotalSize.X, -TotalSize.Y, 0.f);
	case EGridOrigin::Center:
		return -TotalSize * 0.5f;
	default:
		return FVector::ZeroVector;
	}
}

void UGrid::BeginPlay()
{
	Super::BeginPlay();
}


FIntPoint UGrid::Mirror(FIntPoint Cell) const
{
	return FIntPoint(GridSize.X - 1 - Cell.X, Cell.Y);
}

FVector UGrid::CellToWorld(const FIntPoint& Cell) const
{
	if (!GetOwner())return FVector::ZeroVector;
	const FVector ActorLocation = GetOwner()->GetActorLocation();
	const FQuat ActorRotation = GetOwner()->GetActorQuat();
	FVector GridLocal(
		(Cell.X + 0.5f) * CellSize.X,
		(Cell.Y + 0.5f) * CellSize.Y,
		0.f
	);
	GridLocal += GetGridOriginOffset();
	return ActorLocation + ActorRotation.RotateVector(GridLocal);
}

FIntPoint UGrid::WorldToCell(const FVector& WorldPos) const
{
	if (!GetOwner()) return FIntPoint::ZeroValue;
	const FVector ActorLocation = GetOwner()->GetActorLocation();
	const FQuat ActorRotation = GetOwner()->GetActorQuat();
	FVector GridLocal = ActorRotation.Inverse().RotateVector(WorldPos - ActorLocation);
	GridLocal -= GetGridOriginOffset();
	return FIntPoint(
		FMath::FloorToInt(GridLocal.X / CellSize.X),
		FMath::FloorToInt(GridLocal.Y / CellSize.Y)
	);
}

bool UGrid::IsCellValid(const FIntPoint& Cell) const
{
	return Cell.X >= 0 && Cell.Y >= 0 &&
		Cell.X < GridSize.X && Cell.Y < GridSize.Y;
}

bool UGrid::IsWorldPosInsideGrid(const FVector& WorldPos) const
{
	const FIntPoint Cell = WorldToCell(WorldPos);
	return IsCellValid(Cell);
}

#if !UE_BUILD_SHIPPING
void UGrid::DrawDebugCellText() const
{
	for (int x = 0; x < GridSize.X; ++x)
	{
		for (int y = 0; y < GridSize.Y; ++y)
		{
			const FIntPoint Cell(x, y);
			const FVector WorldPos = CellToWorld(Cell);
			DrawDebugString(
				GetWorld(),
				WorldPos + FVector(0.f, 0.f, DebugLineZOffset),
				FString::Printf(TEXT("(%d,%d)"), Cell.X, Cell.Y),
				nullptr,
				DebugTextColor,
				0.f,
				false,
				DebugTextScale
			);
		}
	}
}

void UGrid::DrawDebugGrid() const
{
	const FVector ActorLocation = GetOwner()->GetActorLocation() + FVector(0.f, 0.f, DebugLineZOffset);
	const FQuat ActorRotation = GetOwner()->GetActorQuat();
	for (int32 X = 0; X < GridSize.X; ++X)
	{
		for (int32 Y = 0; Y < GridSize.Y; ++Y)
		{
			const FIntPoint Cell(X, Y);
			const float Width = GridSize.X * CellSize.X;
			const float Height = GridSize.Y * CellSize.Y;
			const FVector OriginOffset = GetGridOriginOffset();
			{
				float LineX = Cell.X * CellSize.X + OriginOffset.X;
				// 画左边界
				DrawDebugLine(
					GetWorld(),
					ActorLocation + ActorRotation.RotateVector(FVector(LineX, 0.f + OriginOffset.Y, 0.f)),
					ActorLocation + ActorRotation.RotateVector(FVector(LineX, Height + OriginOffset.Y, 0.f)),
					DebugLineColor,
					false,
					0.f,
					0,
					DebugLineThickness
				);

				// 最后一列额外画右边界
				if (Cell.X == GridSize.X - 1)
				{
					float RightX = Width + OriginOffset.X;
					DrawDebugLine(
						GetWorld(),
						ActorLocation + ActorRotation.RotateVector(FVector(RightX, 0.f + OriginOffset.Y, 0.f)),
						ActorLocation + ActorRotation.RotateVector(FVector(RightX, Height + OriginOffset.Y, 0.f)),
						DebugLineColor,
						false,
						0.f,
						0,
						DebugLineThickness
					);
				}
			}
			{
				float LineY = Cell.Y * CellSize.Y + OriginOffset.Y;
				// 画下边界
				DrawDebugLine(
					GetWorld(),
					ActorLocation + ActorRotation.RotateVector(FVector(0.f + OriginOffset.X, LineY, 0.f)),
					ActorLocation + ActorRotation.RotateVector(FVector(Width + OriginOffset.X, LineY, 0.f)),
					DebugLineColor,
					false,
					0.f,
					0,
					DebugLineThickness
				);

				// 最后一行额外画上边界
				if (Cell.Y == GridSize.Y - 1)
				{
					float TopY = Height + OriginOffset.Y;
					DrawDebugLine(
						GetWorld(),
						ActorLocation + ActorRotation.RotateVector(FVector(0.f + OriginOffset.X, TopY, 0.f)),
						ActorLocation + ActorRotation.RotateVector(FVector(Width + OriginOffset.X, TopY, 0.f)),
						DebugLineColor,
						false,
						0.f,
						0,
						DebugLineThickness
					);
				}
			}
		}
	}
}
#endif

void UGrid::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
#if WITH_EDITOR
	if (!GetWorld() || !GetOwner())return;
	if (bDrawDebugGrid)DrawDebugGrid();
	if (bDrawDebugCellText)DrawDebugCellText();
#endif
}
